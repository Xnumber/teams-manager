import { CommonModule } from '@angular/common';
import { Component, inject, input } from '@angular/core';
import { toObservable } from '@angular/core/rxjs-interop';
import { DxChartModule } from 'devextreme-angular';
import { combineLatest, from, map, of, switchMap, catchError } from 'rxjs';
import { PlanService } from '../service/plans';

@Component({
  selector: 'app-estimation-histories',
  imports: [CommonModule, DxChartModule],
  templateUrl: './estimation-histories.html',
  styleUrl: './estimation-histories.scss',
})
export class EstimationHistories {
  planId = input<string | null>(null);
  refreshToken = input<number>(0);

  private planService = inject(PlanService);

  convergenceStartDate = new Date();
  convergenceEndDate = new Date();

  planHistory$ = combineLatest([
    toObservable(this.planId),
    toObservable(this.refreshToken),
  ]).pipe(
    switchMap(([planId]) => {
      if (!planId) {
        return of([]);
      }

      return from(this.planService.getPlanHistory(planId)).pipe(
        map((data: any[] | undefined) => {
          const histories =
            data?.map((item) => ({
              x: new Date(item.estimation_date),
              complete_date: new Date(item.complete_date),
              optimistic_estimated_complete_date: new Date(
                item.optimistic_estimated_complete_date,
              ),
              pessimistic_estimated_complete_date: new Date(
                item.pessimistic_estimated_complete_date,
              ),
              left_tasks: item.left_tasks,
            })) ?? [];

          const completeDates = histories
            .map((item: { complete_date: Date }) => item.complete_date)
            .filter((date: Date) => !Number.isNaN(date.getTime()));

          if (completeDates.length > 0) {
            const minCompleteDate = new Date(
              Math.min(...completeDates.map((date: Date) => date.getTime())),
            );
            const maxCompleteDate = new Date(
              Math.max(...completeDates.map((date: Date) => date.getTime())),
            );

            this.convergenceStartDate = new Date(minCompleteDate);
            this.convergenceStartDate.setMonth(
              this.convergenceStartDate.getMonth() - 3,
            );

            this.convergenceEndDate = new Date(maxCompleteDate);
            this.convergenceEndDate.setMonth(
              this.convergenceEndDate.getMonth() + 3,
            );
          }

          return histories;
        }),
        catchError(() => of([])),
      );
    }),
  );

}
