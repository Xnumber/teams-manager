import { CommonModule } from '@angular/common';
import { Component, inject, input } from '@angular/core';
import { toObservable } from '@angular/core/rxjs-interop';
import { DxChartModule } from 'devextreme-angular';
import { combineLatest, from, map, of, switchMap, catchError } from 'rxjs';
import { PlanService } from '../service/plans';

@Component({
  selector: 'app-plan-task-metrics-histories',
  imports: [CommonModule, DxChartModule],
  templateUrl: './plan-task-metrics-histories.html',
  styleUrl: './plan-task-metrics-histories.scss',
})
export class PlanTaskMetricsHistories {
  planId = input<string | null>(null);
  refreshToken = input<number>(0);

  private planService = inject(PlanService);

  metricsHistories$ = combineLatest([
    toObservable(this.planId),
    toObservable(this.refreshToken),
  ]).pipe(
    switchMap(([planId]) => {
      if (!planId) {
        return of([]);
      }

      return from(this.planService.updateMetricsHistory(planId)).pipe(
        switchMap(() => from(this.planService.listMetricsHistories(planId))),
        map((items: any[] | undefined) => {
          return (
            items
              ?.map((item) => ({
                x: new Date(item.date),
                task_count: item.task_count,
                task_added_count: item.task_added_count,
                task_completed_count: item.task_completed_count,
                // milestone_count: item.milestone_count,
                // milestone_added_count: item.milestone_added_count,
                // milestone_completed_count: item.milestone_completed_count,
              }))
              .sort((a, b) => a.x.getTime() - b.x.getTime()) ?? []
          );
        }),
        catchError(() => of([])),
      );
    }),
  );

}
