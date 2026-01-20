import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Чтение данных из файла
df = pd.read_csv('results_external.csv')

# Преобразуем числовые столбцы в числовой формат, игнорируя ошибки
df['Time(ms)'] = pd.to_numeric(df['Time(ms)'], errors='coerce')
df['SpeedUp'] = pd.to_numeric(df['SpeedUp'], errors='coerce')
df['Efficiency'] = pd.to_numeric(df['Efficiency'], errors='coerce')

# Преобразование Nthreads из шестнадцатеричной системы в десятичную
def hex_to_int(x):
    try:
        if isinstance(x, str):
            # Убираем возможные пробелы
            x = x.strip()
            # Пытаемся преобразовать из hex
            try:
                return int(x, 16)
            except ValueError:
                # Если не hex, пробуем как обычное число
                return int(float(x)) if '.' in x else int(x)
        else:
            return int(x)
    except (ValueError, TypeError):
        return np.nan

# Применяем преобразование
df['Nthreads_dec'] = df['Nthreads'].apply(hex_to_int)

# Удаляем строки с NaN значениями
df_clean = df.dropna(subset=['Nthreads_dec', 'Time(ms)', 'SpeedUp', 'Efficiency'])

# Сортируем по алгоритму и количеству потоков
df_clean = df_clean.sort_values(['Algorithm', 'Nthreads_dec'])

# Получение списка уникальных алгоритмов
algorithms = sorted(df_clean['Algorithm'].unique())

# Определение ОБЩИХ пределов для ВСЕХ графиков
time_max = df_clean['Time(ms)'].max() * 1.1
speedup_max = max(df_clean['SpeedUp'].max(), 2.0)
efficiency_max = 1.2

# Определение диапазона потоков для оси X (одинаковый для всех)
thread_min = 0
thread_max = df_clean['Nthreads_dec'].max() * 1.05

# Создаём ОДИН график для каждого алгоритма
for algo in algorithms:
    plt.figure(figsize=(12, 8))
    
    # Фильтрация данных для текущего алгоритма
    algo_data = df_clean[df_clean['Algorithm'] == algo].sort_values('Nthreads_dec')
    
    if len(algo_data) == 0:
        print(f"Нет данных для алгоритма: {algo}")
        continue
    
    # Создаём основную ось для Time и SpeedUp
    ax1 = plt.gca()
    
    # График времени выполнения (синяя линия)
    ax1.plot(algo_data['Nthreads_dec'], algo_data['Time(ms)'], 
            'b-o', linewidth=2, markersize=6, label='Time(ms)')
    
    # График ускорения (красная линия)
    ax1.plot(algo_data['Nthreads_dec'], algo_data['SpeedUp'], 
            'r-s', linewidth=2, markersize=6, label='SpeedUp')
    
    # Настройка первой оси
    ax1.set_xlabel('Количество потоков', fontsize=12)
    ax1.set_ylabel('Время (мс) / Ускорение', fontsize=12)
    ax1.set_title(f'Алгоритм: {algo}', fontsize=14, fontweight='bold')
    ax1.grid(True, alpha=0.3, linestyle='--')
    
    # Устанавливаем ОДИНАКОВЫЕ пределы для всех графиков
    ax1.set_xlim(thread_min, thread_max)
    ax1.set_ylim(0, max(time_max, speedup_max))
    
    # Добавляем вторую ось для Efficiency
    ax2 = ax1.twinx()
    ax2.plot(algo_data['Nthreads_dec'], algo_data['Efficiency'], 
            'g-^', linewidth=2, markersize=6, label='Efficiency')
    
    # Настройка второй оси
    ax2.set_ylabel('Эффективность', fontsize=12, color='green')
    ax2.tick_params(axis='y', labelcolor='green')
    ax2.set_ylim(0, efficiency_max)
    
    # Объединяем легенды с двух осей
    lines1, labels1 = ax1.get_legend_handles_labels()
    lines2, labels2 = ax2.get_legend_handles_labels()
    ax1.legend(lines1 + lines2, labels1 + labels2, loc='upper right', fontsize=10)
    
    # Добавляем сетку для второй оси
    ax2.grid(True, alpha=0.2, linestyle=':')
    
    # Добавляем горизонтальные линии для ориентиров
    ax1.axhline(y=1, color='gray', linestyle='--', alpha=0.5, linewidth=0.8)
    ax2.axhline(y=1, color='gray', linestyle='--', alpha=0.5, linewidth=0.8)
    
    plt.tight_layout()
    
    # Сохраняем график
    filename = f'algorithm_{algo}.png'
    plt.savefig(filename, dpi=300, bbox_inches='tight')
    print(f'Создан график: {filename}')
    
    plt.show()

# Создаём сводную таблицу для быстрого сравнения
print("\n" + "="*80)
print("СВОДНАЯ ТАБЛИЦА РЕЗУЛЬТАТОВ")
print("="*80)

summary_data = []

for algo in algorithms:
    algo_data = df_clean[df_clean['Algorithm'] == algo].copy()
    
    if len(algo_data) == 0:
        continue
    
    # Находим лучшие значения
    min_time = algo_data['Time(ms)'].min()
    threads_at_min_time = int(algo_data.loc[algo_data['Time(ms)'].idxmin(), 'Nthreads_dec'])
    
    max_speedup = algo_data['SpeedUp'].max()
    threads_at_max_speedup = int(algo_data.loc[algo_data['SpeedUp'].idxmax(), 'Nthreads_dec'])
    
    max_efficiency = algo_data['Efficiency'].max()
    threads_at_max_efficiency = int(algo_data.loc[algo_data['Efficiency'].idxmax(), 'Nthreads_dec'])
    
    # Время при 1 потоке
    time_at_1_thread_data = algo_data[algo_data['Nthreads_dec'] == 1]
    time_at_1_thread = time_at_1_thread_data['Time(ms)'].iloc[0] if not time_at_1_thread_data.empty else None
    
    summary_data.append({
        'Algorithm': algo,
        'Time (1t)': f'{time_at_1_thread:.1f} ms' if time_at_1_thread else 'N/A',
        'Best Time': f'{min_time:.1f} ms',
        'At Threads': threads_at_min_time,
        'Best SpeedUp': f'{max_speedup:.3f}',
        'At Threads': threads_at_max_speedup,
        'Best Eff.': f'{max_efficiency:.1%}',
        'At Threads': threads_at_max_efficiency
    })

# Создаём и выводим DataFrame
if summary_data:
    summary_df = pd.DataFrame(summary_data)
    print(summary_df.to_string(index=False))
    print(f"\nВсего алгоритмов: {len(algorithms)}")
    
    # Статистика по данным
    print(f"\nСтатистика по данным:")
    print(f"Всего записей: {len(df_clean)}")
    print(f"Диапазон потоков: {int(df_clean['Nthreads_dec'].min())} - {int(df_clean['Nthreads_dec'].max())}")
    print(f"Диапазон времени: {df_clean['Time(ms)'].min():.1f} - {df_clean['Time(ms)'].max():.1f} мс")
    print(f"Диапазон ускорения: {df_clean['SpeedUp'].min():.2f} - {df_clean['SpeedUp'].max():.2f}")
    print(f"Диапазон эффективности: {df_clean['Efficiency'].min():.3f} - {df_clean['Efficiency'].max():.3f}")
    
    # Сохраняем сводную таблицу
    summary_df.to_csv('algorithms_summary.csv', index=False)
    print(f"\nСводная таблица сохранена в файл: algorithms_summary.csv")
    print(f"Всего создано {len(algorithms)} графиков")
else:
    print("Нет данных для создания сводной таблицы")
