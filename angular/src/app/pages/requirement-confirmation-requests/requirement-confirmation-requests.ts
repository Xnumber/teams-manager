import { HttpClient } from '@angular/common/http';
import { Component, inject } from '@angular/core';
import { DxDataGridModule } from 'devextreme-angular';
import DataSource from 'devextreme/data/data_source';
import { Column } from 'devextreme/ui/data_grid';
import { lastValueFrom } from 'rxjs';
import { Auth } from '../../services/auth';
type TaskAcceptanceRequest = {
  id: string;
  task_id: string;
  requester_id: string;
  created_at: string;
};

@Component({
  selector: 'app-requirement-confirmation-requests',
  imports: [DxDataGridModule],
  templateUrl: './requirement-confirmation-requests.html',
  styleUrl: './requirement-confirmation-requests.scss'
})
export class RequirementConfirmationRequests {
  auth = inject(Auth);
  dataSource: DataSource<TaskAcceptanceRequest>;
  columns: Column[] = [
    { dataField: 'id', caption: '請求編號' },
    { dataField: 'task_id', caption: '任務編號' },
    { dataField: 'requester_id', caption: '請求人員' },
    { dataField: 'created_at', caption: '建立時間', dataType: 'datetime' },
  ];

  constructor(private http: HttpClient) {
    this.dataSource = new DataSource<TaskAcceptanceRequest>({
      load: () => {
        return lastValueFrom(this.http.get<ApiResponse<TaskAcceptanceRequest[]>>('/acceptances', {
          params: {
            requester_id: this.auth.getUserId() || '',
          }
        })).then((res) => {
          if (res?.result === 'ok' && Array.isArray(res.data)) {
            return res.data;
          }

          return [];
        });
      },
    });
  }
}
