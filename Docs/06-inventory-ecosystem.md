# 06. Инвентарная экосистема (контекст плагина)

**Статус:** 🟡 контекст собран по коду; сборка и загрузка модулей проверены 2026-10-29
**Источник:** `Plugins/InventoryEcosystem` (сторонний плагин, автор Alexander Polyakov, версия 1.0, beta)
**Задача документа:** зафиксировать, что уже есть в коде, как оно устроено и где проходит граница
между «берём как есть» и «переделываем под правила мира».

> Это **не** ТЗ на инвентарь. Это карта существующего кода: без неё нельзя ни оценивать объём,
> ни проектировать связь с генерацией мира (`07-worldgen-inventory-bridge.md`).

---

## 1. Что лежит в репозитории (факты)

| Параметр | Значение |
|---|---|
| Путь | `Plugins/InventoryEcosystem` |
| Движок | UE 5.8 (`F:\Engine\UE_5.8`), плагин собран 21.10.2026 |
| Модулей | 14: 9 runtime (включая `ItemSystem`) + 4 editor + 1 шаблон |
| Файлов исходников | 503 (`306` runtime, `188` editor, `9` template) |
| Строк кода | ≈ 40 200 (runtime ≈ 26 450, editor ≈ 13 400) |
| Классов | 148 `UCLASS`, 113 `USTRUCT`, 35 `UENUM` |
| Сеть | 30 `Server`-RPC, 2 `Client`-RPC, 13 `NetMulticast` (в заголовках) |
| Репликация | 62 свойства с `Replicated`, 21 упоминание `SaveGame` |
| Автотестов | **0** (нет `WITH_DEV_AUTOMATION_TESTS` / `IMPLEMENT_SIMPLE_AUTOMATION_TEST`) |

Контент плагина почти пустой: `BP_DefaultItem`, демо-карта `Maps/Dev.umap`, физические материалы
(`PM_Blood`, `PM_Metall`, `Tire_Drag`) и LayerInfo для травы/грязи/камня. **UI инвентаря в плагине нет** —
его нужно писать самим (плагин даёт модель и сеть, не экраны).

### 1.1 Плагин подключён к проекту (проверено сборкой)

| Проверка | Результат |
|---|---|
| `UnrealWorld.uproject` | плагин `InventoryEcosystem` включён (`Enabled: true`) |
| Сборка `UnrealWorldEditor` | ✅ `Result: Succeeded` (UE 5.8, UBA) |
| Загрузка модулей | ✅ все 14 модулей грузятся (`LogModuleManager: InternalLoadLibrary`) |
| `Source/UnrealWorld/UnrealWorld.Build.cs` | зависимостей нет — плагин подключается как плагин, а не как библиотека игрового модуля |
| `UnrealWorldServer` | ⬜ ещё не собирался (отдельная задача) |

### 1.2 Что изменилось при подключении

1. В `UnrealWorld.uproject` добавлена запись `{ "Name": "InventoryEcosystem", "Enabled": true }`.
2. Из папок модулей убраны числовые префиксы: папка = имя модуля (`InventoryCore`, `ItemSystem`, ...).
3. Модуль `ItemSystem` выделен из `InventorySystem` (см. §2.1).
4. Класс `UInventorySettings` переименован в `UItemSettings` (переехал в `ItemSystem`), добавлен
   `CoreRedirects` для совместимости; `InteractProfile` переехал в `UInteractSystemSettings`.

Порядок проверки после изменений: собрать `UnrealWorldEditor` → запустить (в т.ч. headless) →
убедиться, что в логе есть `InternalLoadLibrary: 'ItemSystem'` и нет `Fatal error`.


---

## 2. Карта модулей

Папка модуля совпадает с именем модуля (числовые префиксы убраны 2026-10-29).
Include-пути в коде всегда идут от корня `Source`, например:
`#include "InventoryEcosystem/ItemSystem/Objects/Item/BaseItem.h"`. Эту конвенцию ломать нельзя.

| Модуль | Папка | Тип | Роль | Зависит от |
|---|---|---|---|---|
| `InventoryTools` | `InventoryTools` | Runtime | Мелочи: склейка скелетных мешей, математика, SaveGame-архив, поиск компонентов | `InventoryCore`, `SkeletalMerging`, `PhysicsCore` |
| `InventoryCore` | `InventoryCore` | Runtime | База: `UCoreInventoryObject`, компоненты, интерфейсы, лог | `GameplayTags`, `Engine` |
| `InteractSystem` | `InteractSystem` | Runtime | Взаимодействие: трейс, состояния, анимации, виджет | `InventoryCore`, `InventoryTools`, `UMG` |
| **`ItemSystem`** | `ItemSystem` | Runtime | **Предметы: `UBaseItem`, сервисы, `ABaseItemActor`, фабрики, настройки предметов** | `InventoryCore`, `InteractSystem` |
| `InventorySystem` | `InventorySystem` | Runtime | Только контейнеры: линия / сетка / пространственный | `ItemSystem`, `InventoryTools` |
| `EquipManager` | `EquipManager` | Runtime | Экипировка, слоты по тегам, склейка тела, FPS/TPS-меши | `ItemSystem`, `InventoryTools`, `SkeletalMerging` |
| `HandItemSystem` | `HandItemSystem` | Runtime | Руки, оружие, модули, камера, GAS, комбо-графы, Mover | `ItemSystem`, `GameplayAbilities`, `Mover`, `Niagara`, `EnhancedInput` |
| `LootSystem` | `LootSystem` | Runtime | Лут-таблицы, контейнеры, респавн, торговля, валюты | `ItemSystem`, `InventorySystem`, `InventoryTools` |
| `InventoryManagerSystem` | `InventoryManagerSystem` | Runtime | Вложенные инвентари предметов (сумки, рюкзаки, разгрузки) | `ItemSystem`, `InventorySystem` |
| `InventoryCoreEditor` | `InventoryCoreEditor` | Editor | Кастомные панели деталей (массивы), общие Slate-кнопки | `Slate`, `PropertyEditor`, `UnrealEd` |
| `InventorySystemEditor` | `InventorySystemEditor` | Editor | Генератор иконок-миниатюр предметов (`ItemThumbnail`) | `ItemSystem`, `UnrealEd`, `AssetTools` |
| `HandItemSystemEditor` | `HandItemSystemEditor` | Editor | Полноценный редактор комбо-графов + аним-нода `GameplayTagBlend` | `GraphEditor`, `AnimGraph` |
| `LootSystemEditor` | `LootSystemEditor` | Editor | Отдельный asset-editor для лут-таблиц, пресетов и торговли | `ItemSystem`, `Slate`, `PropertyEditor` |
| `InventoryTemplate` | `Source/InventoryTemplate` | Runtime | Пример сборки: персонаж, контроллер, настройки | все runtime-модули |

Порядок зависимостей (снизу вверх):

```
InventoryCore
└─ InventoryTools
   └─ InteractSystem
      └─ ItemSystem ────────────────┬─ InventorySystem ─┬─ LootSystem
                                    │                   └─ InventoryManagerSystem
                                    ├─ EquipManager
                                    └─ HandItemSystem
```

### 2.1 Что дал разрез `ItemSystem` ← `InventorySystem`

| Было в `InventorySystem` | Стало |
|---|---|
| `Objects/Item/BaseItem.*` | `ItemSystem/Objects/Item/` |
| `Data/InventoryData.*` (`FDefaultItemData`, теги, `UInventoryData`) | `ItemSystem/Data/` |
| `Objects/ItemServices/*` (5 сервисов) | `ItemSystem/Objects/ItemServices/` |
| `Objects/MakeItemRule/*` | `ItemSystem/Objects/MakeItemRule/` |
| `Actor/BaseItemActor.*` | `ItemSystem/Actor/` |
| `Library/InventoryLibrary.*` (`MakeItem*`, `SpawnItemObjectContext`, `FInventoryItemData`) | `ItemSystem/Library/` |
| `Public/InventorySettings.h` (`UInventorySettings`) | `ItemSystem/Public/ItemSettings.h` (`UItemSettings`) |
| `Components/Inventory/*` (линия/сетка/пространственный) | остались в `InventorySystem` |

Результат по зависимостям: `EquipManager` и `HandItemSystem` **больше не зависят от контейнеров**
(проверено: ни одного include из `InventorySystem` в этих модулях), а `InventorySystem` перестал
зависеть от `InteractSystem`.

Плагины-зависимости из `.uplugin`: `GameplayAbilities`, `StructUtils`, `Mover`, `ChaosMover`,
`EnhancedInput`, `CableComponent`. То есть **движение и способности — не «свои»**: персонаж плагина
построен на Mover и GAS (план замены — `Docs/09-combat-system.md`, пока не создан).


---

## 3. Фундамент: `UCoreInventoryObject` (модуль 01)

Любой предмет, сервис, правило и компонент-менеджер — это `UObject`, наследник
`UCoreInventoryObject`, а не актёр.

| Механика | Как устроено | Зачем важно нам |
|---|---|---|
| `Outer` = `AActor` | `GetOwner()` кэширует `Cast<AActor>(GetOuter())` | Предметы «живут» внутри владельца и переезжают вместе с ним |
| `ParrentObject` | Ссылка на «контейнер-владелец», `Replicated` | Один предмет не может лежать в двух местах; перенос — через `SwitchParrent()` |
| `RepOuter` + `OnRep_Outer` | При переносе клиент переименовывает (`Rename`) объект в нового Outer | Клиент и сервер держат одну и ту же иерархию объектов |
| `HasAuthority()` | Прокси к `GetOwner()->HasAuthority()` | Все мутации — только на сервере |
| Сетевой вызов | Переопределены `CallRemoteFunction` / `GetFunctionCallspace` | RPC можно объявлять **на самом предмете**, а не на актёре |
| `IsCanRemoveFromParrent()` | Спрашивает владельца через `IItemManagmentInterface` | Запрет «вытащить из запертого контейнера» реализуется на владельце |

`IItemManagmentInterface` — два метода: `I_RemoveItem(UCoreInventoryObject*)` и
`I_CanRemoveItem(...)`. Его реализуют инвентари, руки, экипировка, контейнеры, актёры предметов.

> ⚠️ Технический момент: предметы — **сабобъекты актора**, которые репликуются вручную через
> `ReplicateSubobjects` у компонентов-инвентарей (`Channel->ReplicateSubobject(Item, ...)`).
> Это работает, но требует, чтобы инвентарь был зарегистрирован как репликуемый компонент.

---

## 4. Предмет (`ItemSystem`)

### 4.1 `UBaseItem` + `FDefaultItemData`

`UBaseItem` — `UObject` (не актёр), `Abstract`, `Blueprintable`. Данные по умолчанию лежат в
`FDefaultItemData` (`Data/InventoryData.h`):

| Поле | Тип | Комментарий |
|---|---|---|
| `ItemName`, `Description`, `Icon` | `FText`, `FText`, `UTexture*` | UI |
| `ActionMenu` | `int32` (битмаска `EMenuAction`) | Экипировать / Использовать / Выбросить / Модифицировать / Разделить / Открыть |
| `ItemType` | `FGameplayTag` | Теги в `InventoryItemType`: Weapon, Module, Cloth, Medicine, Food, Currency |
| `RarityType` | `FGameplayTag` | Редкость (лут и UI) |
| `bIsStack`, `StackSize` | `bool`, `int32` | Стеки |
| `Dimensions` | `FIntPoint` | Сколько клеток занимает (поворот — `bIsRotate`) |
| `Weight` | `int32` | **в граммах** (заявлено, но не используется, см. §13) |
| `ItemLVL` | `int32` | Уровень предмета |
| `ActionText` | `FDataTableRowHandle` | Локализация названий действий |

Рантайм-состояние: `Amount` (`ReplicatedUsing=OnRep_Amount`, `SaveGame`), `bIsRotate`,
`PickupService`, `WorldPlacementService`, `QualityService`, `Services[]`.
Делегаты: `OnItemUpdate`, `OnCaptureUpdate`, `OnItemEquiped`, `OnItemUnequiped`.

### 4.2 Сервисы предмета (расширение без наследования)

`UBaseItemService` (`Replicated ItemOwner`) — точка расширения. Готовые сервисы:

| Сервис | Что делает |
|---|---|
| `UBasePickupService` | `PickupItem(APawn*)` — что происходит при подборе |
| `UBaseWorldItemService` | Представление в мире: `StaticMesh`/`SkeletalMesh` (soft ptr), `OverrideMaterial`, `CustomClass` актёра, `SpawnItem()`, `DestroyFromWorld()`, `Server_DropToWorld()`, `SpawnedReference` (реплицируется) |
| `UBaseQualityService` | Прочность: `MaxQualityStatus`, `QualityStatus`, `ApplyDamage()` (только сервер), `IsBroken()` |
| `UBaseEquipedService` | Пустая заглушка (`UObject`, даже не наследник `UBaseItemService`) — «задел на будущее» |
| `UBaseInventoryItemService` (07) | Даёт предмету собственный инвентарь: `SizeInventory`, `InventoryClass`, `MakeInventory()`, `SafeInventoryState()` |
| `UBaseHandItemService` (04) | Анимации взятия/убирания, `DoEquipLogic` / `DoUnequipLogic` |
| `UBaseWeaponService` / `UBaseWeaponModuleService` (04) | Оружие и обвесы |
| `UBaseTradeService`, `UCurrencyServive` (06) | Участие в торговле |

`GetServiceTyped<T>()` — шаблонный поиск сервиса; `GetAttachedSubObject()` — сабобъекты,
которые тоже надо реплицировать (вложенные предметы оружия и т.п.).


### 4.3 Создание предметов

| Функция | Поведение |
|---|---|
| `UInventoryLibrary::MakeItem(Owner, FInventoryItemData)` | `NewObject` + `BeginPlay()` |
| `UInventoryLibrary::MakeItems(...)` | Разбивает количество по стекам (`StackSize`) |
| `UInventoryLibrary::MakeItem(Owner, FMakeItemContext)` | Через `UMakeItemRule` (кастомная инициализация в BP) |
| `UInventoryLibrary::SpawnItemObjectContext(...)` | Спавн `ABaseItemActor` (`SpawnActorDeferred` + `SetupItem`) |

`FInventoryItemData` содержит `FIntPoint Range` и `bIsRange`: количество может быть случайным.
**Сейчас случайность — `FMath::RandRange`** (см. §13, риск №1).

### 4.4 Представление в мире: `ABaseItemActor`

`ABaseItemActor : ABaseInteractedActor, IItemManagmentInterface` — «предмет на земле»:
`ItemRef` (`ReplicatedUsing=OnRep_Item`), `FMakeItemContext ItemContext`, асинхронная загрузка
`StaticMesh`/`SkeletalMesh`, `SetupItem()`, реакция на взаимодействие (`Event_Interacted`).

---

## 5. Инвентари (`InventorySystem/Components/Inventory`)

Три уровня, наследуются друг от друга:

| Класс | Форма | Ключевое |
|---|---|---|
| `UBaseLineInventory` | Лента / слоты | `FNetInventoryData` (`TArray<UBaseItem*>` + размер), `InventoryWeight`, кэш `FInventoryCachedData` |
| `UBaseGridInventory` | Сетка N×M | `GetRows()/GetColums()`, `ConvertIndexToPosition` / `ConvertPositionToIndex`, `static GetGridInventory(AActor*)` |
| `UBaseSpatialInventory` | Сетка с габаритами | Предмет занимает `Dimensions` клеток; `IsPositionAvailable`, `AddItemAt`, `RemoveItemAt`, `CanAddItem` |

Публичный API `UBaseLineInventory` (наследуется всеми уровнями):

- добавление: `AddTryItem(Item, IsNeedStackItem, WithDelegate)`, `AddItemAt(Item, TopLeftIndex, ...)`;
- удаление: `RemoveItemByItem`, `RemoveItemAt`;
- перемещение: `MoveItem`, `SwapItem`, `CanSwap`, `CanAddItem`;
- доступ: `GetItemAtIndex`, `GetAllItem`, `GetAllItemGrid`, `GetFirstItemByClass`, `GetAllCountByClass`, `SubstructItem`;
- размер: `SetSize`, `GetSize`, `ClearInventory`;
- сохранение: `GetSaveData()` / `LoadSaveData(FInventorySave)`.

Сеть:

- `InventoryData` и `InventoryWeight` — `Replicated`;
- клиент не мутирует: шлёт `Server_AddTryItem`, `Server_AddItemAt`, `Server_RemoveItemAt`,
  `Server_SwapItem`, `Server_MoveItem`, `Server_SubstructItem`;
- `ReplicateSubobjects()` пишет предметы и их `GetAttachedSubObject()` в канал;
- `OnRep_Inventory` инвалидирует кэш и через 10 мс вызывает `InventoryUpdate()` — защита от шторма
  обновлений.

Делегаты: `OnUpdateInventory`, `OnUpdateInventoryItems`, `OnNewItem`, `OnLeaveItem`, `OnWeightChange`.

Сохранение: `FInventorySave` (`Saved`, `Size`, `SavedData[]`); запись — `FInventoryItemSave`
(`TSubclassOf<UBaseItem> SavedClass` + `TArray<uint8> ItemData`, сериализуется самим предметом).


---

## 6. Взаимодействие (`InteractSystem`)

| Класс | Роль |
|---|---|
| `ABaseInteractedActor` | Актёр с `UBaseInteractComponent` (родитель для `ABaseItemActor`, контейнеров) |
| `UBaseInteractComponent` | «Сюда можно взаимодействовать»: настройки, фокус, прогресс, оверлей |
| `UBaseInteractManager` | На стороне персонажа: трейс камеры, выбор цели, стейт-машина, движение к точке |
| `UInteractAnimationComponent` | `USkeletalMeshComponent` с точкой/радиусом подхода и правилом анимации |
| `UAnimationInteractRule` | Правило: монтажи, тайминги, отмена |
| `UBaseInteractWidget` | Базовый виджет прогресса (UI рисуем сами) |

`UBaseInteractComponent` (важные поля `FInteractSettings`):

| Параметр | Значение по умолчанию | Смысл |
|---|---|---|
| `Priority` | 1 | Кто выигрывает при нескольких целях |
| `ActionMenu` | битмаска `EInteractType` | Press / Release / Pressed (зажать) / CirculPressed / Released / QTE |
| `InteractionTime` | 2 с | Длительность |
| `TypeTimeDegradation` | Clear / Reverse | Что с прогрессом при отпускании |
| `InteractionDistance` | 700 | Дистанция |
| `bIsOnlyOneInteractor` | false | Монопольное взаимодействие |
| `AngleSettings` | угол + смещение | Сектор, из которого можно взаимодействовать |

`UBaseInteractManager` (на персонаже): трейс 10 Гц (`InteractionCheckFrequency`), `MaxInteractDistance`
500 см, сфера `InteractionSphereSize` 30 см, состояния `ECharacterInteractState`
(Idle → MovingToInteractPoint → InteractState → AwaitPlayerInput), серверные RPC
`Server_BeginInteract` / `Server_EndInteract` / `Server_BeginFocus` / `Server_EndFocus` /
`Server_CancelInteraction`, клиентские `Client_CancelInteraction`, мультикаст анимаций.

Пресеты взаимодействий — `FInteractPresset` в DataTable (`UInteractSystemSettings::InteractPresets`),
плюс `PreviewInteractMesh` для превью. Отладочный cvar: `IES.Interact.ShowInteractTrace`.

---

## 7. Экипировка и внешний вид (`EquipManager`)

| Класс | Роль |
|---|---|
| `UBaseEquipManager` | Слоты экипировки (по `FGameplayTag`), `EquipArray`, предметы по умолчанию, расчёт брони, `RecalculateBody()` |
| `UBaseCharacterEditor` | Кастомизация тела: `Gender`, `VisualData` (DataTable), `GetMeshByBodyType` |
| `UCharacterMeshManager` | Собирает меш персонажа: FPS/TPS-виды, `ViewMesh`, `ShadowMesh`, `SetFPSMode` |
| `UMeshManager` (00) | Склейка скелетных мешей: `AddMesh`/`RemoveMesh`/`BuildMesh`, `bDoNeedPhysicOverride`, `BaseSkeleton` |
| `UBaseClothService`, `UBaseAttachClothService` | Данные одежды: меши, материалы, сокеты |

Сеть: `EquipArray` (`ReplicatedUsing=OnRep_EquipArray`), `Server_SetClothItem`,
`Server_RemoveItemByItem`; меши реплицируются через `FReplicatedMeshComponentsArray`
(`FMeshComponentData` + `FMaterialInfo`, soft-ссылки; на dedicated server загрузка пропускается).

Сохранение: `FEquipManagerSave` → `FEquipManagerSlotSave` (класс + байты предмета).

---

## 8. Руки, оружие и бой (`HandItemSystem`, самый большой модуль: 68 `UCLASS`)

| Блок | Классы | Роль |
|---|---|---|
| Слоты рук | `UBaseHandItemManager`, `FHandSlots`, `FHandSlotSetting` | До 9 слотов, состояние `FHandItemState` (текущий/целевой предмет), `EquipItem`, `SetupItem`, `UnequipCurrentActor` |
| Действия | `UBaseHandItemManager` | `FirstAction`, `SecondAction`, `MeleeAction`, `ReloadAction`, `SwitchScope`, `NotifyEquip` |
| Оружие | `ABaseWeapon`, `ABaseFireWeapon`, `ABaseBulit`, `ABaseWeaponModule`, `ABaseWeaponScope` | Стрельба, отдача, патроны, модули, прицелы |
| Сервисы | `UBaseHandItemService`, `UBaseWeaponService`, `UBaseWeaponModuleService` | Данные оружия, сокеты модификаций (`FWeaponSocketInfo`), совместимые патроны (`SupportedAmmo`) |
| Способности | `UContextAbilitySystemComponent`, `UContextGameplayAbility`, `BaconContextAbilityModule` | GAS: контекстные способности |
| Комбо | `UComboManagerComponent`, `UComboGraphAsset`, `UComboGraphInstance`, `FCombo*NodeData` | Граф комбо: Input → Root → Execution → Jump/Portal/Reset/Condition |
| Камера | `UInventoryCameraComponent` | FPS/TPS, плечо, FOV, лаг пивота, трассы против геометрии, debug-отрисовка |
| Движение | `ABaseCharacter` (`APawn`, `IMoverInputProducerInterface`), `URealisticMoverComponent`, `URealistic*Mode` | Движение на плагине Mover, режимы Ground/Air/Walk |
| Анимации | `UInventoryAnimInstance`, `UBaseWeaponAnimationInstance`, `UAnimationDrivenComponent` | Locomotion, отдача, anim-driven нотифаи |

Важно: `ABaseCharacter` наследуется **от `APawn`, а не от `ACharacter`** — стандартный
`CharacterMovementComponent` не используется. Это надо учитывать при интеграции с нашей моделью
рельефа и коллизией (см. `04-terrain-and-collision.md`): ходьба/склоны/провалы настраиваются
в Mover, а не в `UCharacterMovementComponent`.

Комбо-графы редактируются своим редактором (`HandItemSystemEditor`), ассет — `UComboGraphAsset`.


---

## 9. Лут, контейнеры и торговля (`LootSystem`)

### 9.1 Данные (ассет `ULootData : UPrimaryDataAsset`)

| Коллекция | Тип | Что внутри |
|---|---|---|
| `LootSettings` | `TMap<FName /*GUID*/, FLootSettingsData>` | Настройки конкретного объекта: метаданные, `bUsePreset` + `PresetGUID`, либо собственный `FGenerateSettings` |
| `LootPresets` | `TMap<FName, FLootPressetData>` | Переиспользуемые пресеты |
| `LootData` | `TMap<FName /*Row*/, FLootRowData>` | «Строки» лута: список `FRandomItemData` (вес + `FMakeItemContext`) |

`FGenerateSettings` (сердце лута):

- `bIsUniqueLoot` — лут уникален для игрока;
- `RespawnSettings` → `EGenerationLootType` (None / OnlyOne / BeginPlay / Timer / TimerAfterPickup /
  DailyTime) + `TimeToRestart` (секунды);
- `GenerateBlocks[]` → `FGenerateSettingsBlocks`: `EnableSetting` (Enabled / EnableIf / DisableIf),
  `LootTag`, `GenerateType` и настройки одного из трёх алгоритмов.

| Алгоритм | Структура | Как работает |
|---|---|---|
| Вероятный (`EGIT_Probable`) | `FProbableGenerationSettings` → ячейки с `Percent` | Каждая ячейка независимо: «повезло с шансом — добавили» |
| Случайный (`EGIT_Random`) | `FRandomRowsData` → строки с `Weight` + `FAmountGenerateSettings` | Взвешенный выбор строк, добор до количества (`Min`/`Max`), режимы дубликатов (`ETypeCheckClass`: исключать / суммировать / игнорировать) |
| Прогрессивный (`EGIT_Progressive`) | `FProgressiveGenerationSettings` (TMap очков → категории) + `UScoreGenerateRule` | Лут зависит от «очков» игрока: `UScoreGenerateRule::GetPlayerScore`, `GetIngoreListClass` |

### 9.2 Выдача лута

`ULootWorldSubSystem` (`UWorldSubsystem`) — единственное место расчёта: `GenerateItems`,
`GenerateItem`, `GenerateItemsData`. Возвращает `FMakeItemContext`, из которых затем
`UInventoryLibrary::MakeItems` создаёт предметы.

`ULootObjectGUID` — «паспорт» объекта: `GUID` (`FName`), лениво подтягивает настройки из
`ULootData` (по GUID, с учётом пресета), умеет `AddToLootSystem()` (редакторная кнопка,
создаёт GUID и запись в ассете).

`UBaseLootComponent` — компонент на контейнере/актёре: `GenereteItems(For, WithLoad)`,
`LoadData(...)`, `SaveDataFor(...)`, `InitToLoodData()`.

`ABaseContainer` — актёр-контейнер:

- `Inventory` + `UniqInventoryes[]` — инвентарь «общий» и «персональные» (для уникального лута);
- триггер-сфера + `Date_RespawnLoot`, таймеры респавна;
- `EGLT_BeginPlay` — лут создаётся на старте (для уникального — по событию входа игрока);
- `UniquePickuped` — сохраняет состояние контейнера через `UBaseLootManager` игрока.

`UBaseLootManager` (на `PlayerController`) хранит `LootTags` и `ContainerSaveData`
(`FContainerData`: `ContainerName`, `AlwaysLoad`, `TimeRestart`, `FInventorySave ContainerSave`),
методы `SetLootTag` / `RemoveLootTag` / `AddBy` / `GetDataBy`, RPC `Server_SetLootModifyLVL`.

### 9.3 Торговля

| Класс | Роль |
|---|---|
| `UBaseTradeComponent` | На NPC/контейнере: `FCurrencyTraderData`, `GetTradeCurrency`, `GetScore` |
| `UBaseTradeManager` | На игроке: `FTradeState`, `FCurrenciesCalculation`, `OpenTrade_TradeComponent` / `OpenTrade_Inventory`, `Server_OpenTrade_*` |
| `UTradeSettings`, `UTradeData`, `UCurrencyData` | Ассеты правил: типы торговли (`ETradeType`), валюты, `FTradeModificate` (модификаторы покупки/продажи) |
| `UBaseTradeService`, `UCurrencyServive` | Сервисы предметов: цена предмета, валютная операция |

Валюты и цены задаются **словарями `FName`-тегов** (`FCurrencyType`, `FTradeType`,
`FCurrencyValueByScore`), а не enum — правятся в отдельном редакторе (`LootSystemEditor`).

---

## 10. Вложенные инвентари (`InventoryManagerSystem`)

`UBaseInventoryManager` — компонент на персонаже/контроллере: список `FClothInventoryData`
(`ItemOwner` + `Inventory`), `ReplicatedUsing=OnRep_Inventory` с условием **`COND_OwnerOnly`**.

API: `AddInventory` / `RemoveInventory` / `FindInventory` / `CanAddItem` / `AddTryItem` /
`SubstructItem` / `GetAllItemByClass` / `GetAllItemByType` / `GetAllCountByClass`.

Инвентарь создаётся сервисом предмета (`UBaseInventoryItemService::MakeInventory`) — то есть
«сумка» — это предмет с инвентарём, а не отдельный актёр.


---

## 11. Сетевая модель (сводка)

| Данные | Кто авторитет | Как доходит до клиента |
|---|---|---|
| Предметы в инвентаре | Сервер | Сабобъекты актора (`ReplicateSubobjects`) + `Server_*` RPC на мутации |
| Внешний вид предмета в мире | Сервер | `ABaseItemActor::ItemRef` + `OnRep_Item` |
| Экипировка | Сервер | `EquipArray` + `OnRep_EquipArray`; меши — `FReplicatedMeshComponentsArray` |
| Слоты рук | Сервер | `FHandSlots` + `OnRep_Slots`, `NetMulticast_EquipItem` для анимаций |
| Взаимодействие | Сервер | `Server_BeginInteract` / `EndInteract`, мультикаст анимаций |
| Лут в контейнере | Сервер | Инвентарь контейнера; персональные — `UniqInventoryes` + `IsNetRelevantFor` |
| Вложенные инвентари | Сервер | `COND_OwnerOnly` — только владельцу |
| Торговля | Сервер | `FTradeState` + `Server_OpenTrade_*` |
| Следы/эффекты стрельбы | Локально | Косметика, `FMath::Rand` допустим |

Правила, которые уже соблюдены в коде и которые нельзя ломать:

1. Мутации только через `Server_*`-RPC (клиент описывает намерение, сервер проверяет).
2. Предмет живёт в `Outer`-актёре; перенос = `SwitchParrent` + репликация `RepOuter`.
3. `IsNetRelevantFor` у контейнера отсекает игроков, которым контейнер «не положен».
4. Есть отладочные cvars и логи (`LOG_BIO`, `IES.Interact.ShowInteractTrace`).

---

## 12. Сохранения

| Что | Где | Формат |
|---|---|---|
| Инвентарь | `UBaseLineInventory::Serialize` (только `IsSaveGame()`, только авторитет) | `FInventorySave` + сериализация каждого `UBaseItem` |
| Предмет | `UBaseItem::Serialize` | `Amount`, сервисы, вложенные данные |
| Экипировка | `UBaseEquipManager::Serialize` | `FEquipManagerSave` |
| Слоты рук | `UBaseHandItemManager::Serialize` | `FHandItemManagerSave` |
| Оружие | `UBaseWeaponService` / `UBaseWeaponModuleService` | `FWeaponServiceSave`, `FWeaponModuleServiceSave` |
| Контейнеры | `UBaseLootManager::ContainerSaveData` (`UPROPERTY(SaveGame)`) | `FContainerData` + `FDateTime TimeRestart` |
| Вложенные инвентари | `UBaseInventoryItemService::Serialize` | `FInventoryManagerSlotSave` |

`CanSerialize()` (одинаковая в нескольких классах) запрещает сохранение без движка/владельца,
для archetype, `EditorPreview` и **без авторитета**.

`UBaseInventoryManager::Serialize` **сейчас отключён** (весь код закомментирован, стоит `return;`),
поэтому сохранение вложенных инвентарей через менеджер фактически не работает — только через
`UBaseInventoryItemService`.

---

## 13. Найденные риски и долги (проверено по коду)

| № | Проблема | Факты | Что делать |
|---|---|---|---|
| 1 | **Недетерминированная случайность в луте** | 22 вызова `FMath::Rand`; критичные: `LootWorldSubSystem.cpp:102` (шанс), `:223`, `:254`, `:307` (количество), `:441`; `InventoryLibrary.h:42`, `.cpp:46`, `:89` | Заменить на `Hash64(seed, Purpose::Loot, ...)` из `01-determinism.md`; лут считает только сервер, клиент получает результат состоянием |
| 2 | **GUID лута создаётся случайно** | `ULootObjectGUID::AddToLootSystem()` → `FGuid::NewGuid()` (редакторная кнопка) | Для процедурных контейнеров GUID = детерминированный хеш от (сид, участок, индекс структуры); см. `07-worldgen-inventory-bridge.md` |
| 3 | **Вес не считается** | `UpdateWeight()`: `InventoryWeight += 0; // LItem->GetWeight();`; метода `GetWeight` у предмета нет вовсе | Реализовать вес: `Weight` (граммы) × `Amount`, `int32` |
| 4 | **Дробные величины в геймплее** | `InventoryWeight` (float), `Percent` (float), `TimeToRestart` (float), `ModPlayerBuy/Sell` (float) | Перевести в целые (`г`, `0…10000`, `мс`, `0…10000`) |
| 5 | **Нет версий данных** | В плагине нет ни `FORMAT_VERSION`, ни `LOOT_VERSION`, ни хешей | Ввести `LOOT_VERSION` (строки/пресеты/правила) и учесть в `WORLD_VERSION` |
| 6 | **Три `DeveloperSettings` с одинаковым именем** | `UInventorySettings`, `UInteractSystemSettings`, `ULootSystemSetting` — все `DisplayName = "Inventory System"` | Переименовать в Project Settings |
| 7 | **Сохранение вложенных инвентарей отключено** | `UBaseInventoryManager::Serialize` — код в комментарии | Включить и покрыть тестом либо признать единственным путём `UBaseInventoryItemService` |
| 8 | **Пустые заглушки контейнеров** | `RegisterLoginPlayer`, `RegisterLogoutPlayer`, `RunRespawn`, `SetMultiOwner`, `AddRelevant` — пустые | Дописать при интеграции с сервером (иначе уникальный лут не выдаётся при входе) |
| 9 | **Нет автотестов** | 0 automation-тестов | Golden-хеши пишем сами (требование `01` §6) |
| 10 | **Плагин не подключён** | ✅ **выполнено 2026-10-29**: плагин включён, Editor собирается, модули грузятся | Осталось собрать `UnrealWorldServer` |
| 11 | **Жёсткая связка с Mover + GAS** | `ABaseCharacter : APawn`, `IMoverInputProducerInterface`, `UContextAbilitySystemComponent` | Проверить headless-сервер: камера/анимации/скинледж не должны ломать DS |
| 12 | **Потерянный русский текст** | 4 файла с `U+FFFD` (текст уничтожен ещё до нас): `ItemSystem/Objects/Item/BaseItem.h`, `BaseItem.cpp`, `ItemSystem/Objects/ItemServices/BaseWorldItemService.h`, `InventorySystem/Components/Inventory/BaseLineInventory.h`. BOM приведён у всех исходников (0 файлов без BOM с не-ASCII) | Переписать комментарии в этих 4 файлах; правило — `Docs/conventions.md` |
| 13 | **UI отсутствует** | В `Content/Blueprint` только `BP_DefaultItem` | Экраны инвентаря/контейнера/торговли делать самим (UMG) |
| 14 | **Нет контракта «предмет ↔ мир»** | Позиция предмета в мире — в актёре, не в данных участка | Нужен слой «что лежит в этом участке» (см. `07`) |


---

## 14. Что берём как есть, что меняем, что пишем сами

| Решение | Объект | Обоснование |
|---|---|---|
| ✅ Берём | Ядро предметов, инвентари (линия/сетка/пространственный), сеть, экипировка, руки, оружие, торговля, лут-алгоритмы, редакторы | ≈ 40 тыс. строк и полный серверный цикл; писать заново — месяцы |
| 🟡 Патчим | `ULootWorldSubSystem` (RNG), `ULootObjectGUID` (GUID), вес и целые числа, `UBaseInventoryManager::Serialize`, пустые заглушки контейнеров | Точечные правки, изолированные в 4–5 файлах |
| 🟠 Дописываем | UI (инвентарь, контейнер, торговля, HUD взаимодействия), сохранение мира (интеграция с Tier C), ограничения по весу/объёму | Этого в плагине нет вообще |
| ❌ Не берём | Демо-персонаж/карту плагина как основу проекта, «голый» `ULootData` без версий | Конфликтует с рантайм-миром и правилами детерминизма |

---

## 15. Чек-лист интеграции (по шагам)

1. Включить плагин в `UnrealWorld.uproject`, добавить зависимости в `UnrealWorld.Build.cs`,
   собрать (Editor + Server), проверить, что Mover/ChaosMover/GAS поднимаются.
2. Переименовать `DeveloperSettings` (риск №6), вынести настройки в `Config/DefaultGame.ini`.
3. Пропатчить RNG лута (риск №1) и GUID (риск №2) — до первого процедурного контейнера.
4. Ввести `LOOT_VERSION` и включить его в `WORLD_VERSION`.
5. Реализовать вес (риск №3) и перевести проценты/цены в целые (риск №4).
6. Написать UI-минимум: сетка инвентаря, контейнер, экшен-меню, HUD взаимодействия.
7. Связать контейнеры с раскладкой структур мира (`07`).
8. Покрыть тестами: детерминизм лута, сохранение/загрузка, перенос предмета между инвентарями.

---

## 16. Быстрая карта файлов

| Что ищем | Файл |
|---|---|
| База объекта | `InventoryCore/Object/CoreInventoryObject.h` |
| Интерфейс владельца | `InventoryCore/Interface/ItemManagmentInterface.h` |
| Предмет | `ItemSystem/Objects/Item/BaseItem.h`, `ItemSystem/Data/InventoryData.h` |
| Настройки предметов | `ItemSystem/Public/ItemSettings.h` |
| Инвентари | `InventorySystem/Components/Inventory/BaseLineInventory.h` (+ Grid/Spatial) |
| Фабрики предметов | `ItemSystem/Library/InventoryLibrary.h` |
| Взаимодействие | `InteractSystem/Components/BaseInteractComponent.h`, `BaseInteractManager.h` |
| Экипировка | `EquipManager/Component/BaseEquipManager.h`, `CharacterMeshManager.h` |
| Руки/оружие | `HandItemSystem/Components/HandItemManager/BaseHandItemManager.h` |
| Лут-данные | `LootSystem/Data/LootSystemData.h` |
| Расчёт лута | `LootSystem/WorldSubSystem/LootWorldSubSystem.cpp` |
| Контейнер | `LootSystem/Actor/BaseContainer.h/.cpp` |
| Торговля | `LootSystem/Component/Trade/*`, `Data/TradeData.h`, `Data/CurrencyData.h` |
| Вложенные инвентари | `InventoryManagerSystem/Component/BaseInventoryManager.h` |
| Пример сборки | `Source/InventoryTemplate/Character/BaseInventoryCharacter.h` |

---

## 17. Открытые вопросы

1. Оставляем ли `UObject`-предметы с ручной репликацией сабобъектов или переводим «тяжёлые»
   инвентари (склады, трупы) на структуры данных? (влияет на трафик и память)
2. Где хранится «что лежит в участке»: в данных участка (`structures[]`) или в отдельном
   реестре сущностей сервера?
3. Нужен ли уникальный лут на игрока в мире-песочнице (сейчас — `bIsUniqueLoot` + персональные
   инвентари контейнера)?
4. Как частота респавна лута соотносится с «правками игроков» (`Tier C`)?
5. Пишем ли свой слой «вес/объём» или ограничиваемся весом в граммах из плагина?

