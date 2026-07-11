export type WorkItemDependencyType =
  | 'FS'
  | 'SS'
  | 'FF'
  | 'SF';

export type WorkItemType =
  | 'PROJECT'
  | 'TASK'
  | 'MILESTONE';



export enum DependencyType {
  FS = 'FS', // Finish to Start
  SS = 'SS', // Start to Start
  FF = 'FF', // Finish to Finish
  SF = 'SF', // Start to Finish
}


export interface WorkItemDependency {
  id?: string;
  predecessorType: WorkItemType;
  predecessorId: string;
  successorType?: WorkItemType;
  successorId?: string;
  dependencyType: DependencyType;
  createdAt?: string;
}