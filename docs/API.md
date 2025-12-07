# API Documentation

## Core Classes

### Option

Класс для описания опциона.

```cpp
namespace pricing::core {

enum class OptionType {
    Call,
    Put
};

class Option {
public:
    Option(OptionType type, double strike, double timeToExpiration);
    
    OptionType getType() const;
    double getStrike() const;
    double getTimeToExpiration() const;
    bool isCall() const;
    bool isPut() const;
};

}
```

**Параметры конструктора:**
- `type` - Тип опциона (Call или Put)
- `strike` - Цена страйк (должна быть положительной)
- `timeToExpiration` - Время до экспирации в годах (неотрицательное)

**Исключения:**
- `std::invalid_argument` - если strike <= 0 или timeToExpiration < 0

### MarketData

Класс для хранения рыночных данных.

```cpp
namespace pricing::core {

class MarketData {
public:
    MarketData(double spot, double riskFreeRate, double volatility);
    
    double getSpot() const;
    double getRiskFreeRate() const;
    double getVolatility() const;
};

}
```

**Параметры конструктора:**
- `spot` - Текущая цена базового актива (должна быть положительной)
- `riskFreeRate` - Безрисковая процентная ставка (годовая)
- `volatility` - Волатильность (годовая, неотрицательная)

**Исключения:**
- `std::invalid_argument` - если spot <= 0 или volatility < 0

### PricingResult

Структура для результата расчёта цены опциона.

```cpp
namespace pricing::core {

struct PricingResult {
    double price = 0.0;
    double delta = 0.0;
    double gamma = 0.0;
    double vega = 0.0;
    double theta = 0.0;
    double rho = 0.0;
    
    bool hasGreeks() const;
};

}
```

**Поля:**
- `price` - Рассчитанная цена опциона
- `delta` - Чувствительность к изменению цены базового актива
- `gamma` - Чувствительность дельты к изменению цены
- `vega` - Чувствительность к изменению волатильности
- `theta` - Временное убывание (обычно отрицательное)
- `rho` - Чувствительность к изменению безрисковой ставки

## Pricing Models

### PricingModel

Интерфейс для моделей прайсинга.

```cpp
namespace pricing::models {

class PricingModel {
public:
    virtual ~PricingModel() = default;
    virtual core::PricingResult price(
        const core::Option& option,
        const core::MarketData& marketData) const = 0;
};

}
```

### BlackScholesModel

Реализация модели Блэка-Шоулза.

```cpp
namespace pricing::models {

class BlackScholesModel : public PricingModel {
public:
    core::PricingResult price(
        const core::Option& option,
        const core::MarketData& marketData) const override;
    
    core::PricingResult priceWithGreeks(
        const core::Option& option,
        const core::MarketData& marketData) const;
};

}
```

**Методы:**
- `price()` - Рассчитывает только цену опциона
- `priceWithGreeks()` - Рассчитывает цену и все греки

**Пример использования:**
```cpp
#include "pricing/core/Option.hpp"
#include "pricing/core/MarketData.hpp"
#include "pricing/models/BlackScholesModel.hpp"

using namespace pricing;

core::Option option(core::OptionType::Call, 100.0, 0.5);
core::MarketData marketData(100.0, 0.05, 0.2);
models::BlackScholesModel model;

auto result = model.priceWithGreeks(option, marketData);
std::cout << "Price: " << result.price << std::endl;
std::cout << "Delta: " << result.delta << std::endl;
```

## Греки (Greeks)

### Delta (Δ)
Изменение цены опциона при изменении цены базового актива на 1 единицу.
- Для Call: 0 ≤ Δ ≤ 1
- Для Put: -1 ≤ Δ ≤ 0

### Gamma (Γ)
Изменение дельты при изменении цены базового актива на 1 единицу.
- Всегда положительна
- Одинакова для Call и Put с одинаковыми параметрами

### Vega (ν)
Изменение цены опциона при изменении волатильности на 1% (0.01).
- Всегда положительна
- Одинакова для Call и Put с одинаковыми параметрами

### Theta (Θ)
Изменение цены опциона при уменьшении времени до экспирации на 1 день.
- Обычно отрицательна (временное убывание)

### Rho (ρ)
Изменение цены опциона при изменении безрисковой ставки на 1% (0.01).
- Положительна для Call
- Отрицательна для Put
