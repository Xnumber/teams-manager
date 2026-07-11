import type { ProjectMetricsHistory } from './type';

export function aggregateCountByDate<
  K extends keyof ProjectMetricsHistory
>(
  histories: ProjectMetricsHistory[],
  keys: K[],
): Array<{ date: string } & Record<K, number>> {
  const map = histories.reduce(
    (acc, item) => {
      if (!acc[item.date]) {
        acc[item.date] = {} as Record<K, number>;
        keys.forEach((key) => {
          acc[item.date][key] = 0;
        });
      }

      keys.forEach((key) => {
        acc[item.date][key] += Number(item[key] ?? 0);
      });

      return acc;
    },
    {} as Record<string, Record<K, number>>,
  );

  return Object.entries(map)
    .map(([date, values]) => ({
      date,
      ...values,
    }))
    .sort((a, b) => a.date.localeCompare(b.date));
}