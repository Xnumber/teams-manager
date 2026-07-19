import { Routes } from '@angular/router';
// import { importMetaGlob } from './utils/importMetaGlob';

// const pages = importMetaGlob([
//     './pages/**/*.ts',
// ])

// console.log(pages);







export const routes: Routes = [
    {
        path: 'qwertyuiop2',
        title: 'qwertyuiop2',
        loadComponent: () => import('./pages/user-profile/summary-filter-user/summary.component').then(m => m.UserProfileSummaryComponent)
    },
    {
        title: "概觀",
        path: 'overview',
        loadComponent: () => import('./pages/projects-preview/projects').then(m => m.Projects)
    },
    {
        title: "專案動態推估",
        path: 'analytics',
        loadComponent: () => import('./pages/analytics/analytics').then(m => m.Analytics)
    },
    {
        title: "個人",
        path: 'user-profile',
        loadComponent: () => import('./pages/user-profile/user-profile').then(m => m.UserProfile),
        children: [
            {
                path: '',
                title: 'Overview',
                loadComponent: () => import('./pages/user-profile/summary/summary.component').then(m => m.UserProfileSummaryComponent)
            },
            {
                path: 'completed-tasks',
                title: 'Completed Tasks',
                loadComponent: () => import('./pages/user-profile/completed-tasks/completed-tasks').then(m => m.CompletedTasks)
            },
            {
                path: 'incomplete-tasks',
                title: 'Incomplete Tasks',
                loadComponent: () => import('./pages/user-profile/incomplete-tasks/incomplete-tasks').then(m => m.IncompleteTasks)
            },
            {
                path: 'settings',
                title: 'Settings',
                loadComponent: () => import('./pages/user-profile/settings/settings').then(m => m.Settings)
            },
            {
                path: 'project-settings',
                title: 'Project Settings',
                loadComponent: () => import('./pages/user-profile/project-settings/project-settings').then(m => m.ProjectSettings)
            }
            // {
            //     path: 'mission-stats',
            //     title: '完成任務統計',
            //     loadComponent: () => import('./pages/user-profile/mission-stats/mission-stats.component').then(m => m.MissionStatsComponent)
            // },
            // {
            //     path: 'quiz-stats',
            //     title: '測驗統計',
            //     loadComponent: () => import('./pages/user-profile/quiz-stats/quiz-stats.component').then(m => m.QuizStatsComponent)
            // },
            // {
            //     path: 'user-tasks',
            //     title: '個人任務',
            //     loadComponent: () => import('./pages/user-profile/user-tasks/user-tasks.component').then(m => m.UserTasksComponent)
            // },
        ]
    },
    {
        title: "首頁",
        path: '',
        loadComponent: () => import('./pages/home/home').then(m => m.Home)
    },
    {
        title: "Register",
        path: 'register',
        loadComponent: () => import('./pages/register/register').then(m => m.Register)
    },
    {
        title: "Login",
        path: 'login',
        loadComponent: () => import('./pages/login/login').then(m => m.Login)
    },
    {
        title: "Tenants",
        path: 'tenants',
        loadComponent: () => import('./pages/tenants/tenants').then(m => m.Tenants)
    },





    
    {
        title: "團隊",
        path: 'teams',
        loadComponent: () => import('./pages/teams/teams').then(m => m.Teams)
    },
    {
        title: "專案",
        path: 'projects',
        loadComponent: () => import('./pages/projects/projects').then(m => m.Projects)
    },
    {
        title: "工作",
        path: 'tasks',
        loadComponent: () => import('./pages/tasks/tasks').then(m => m.Tasks)
    },
    {
        title: "Task Types",
        path: 'task-types',
        loadComponent: () => import('./pages/task-types/task-types').then(m => m.TaskTypes)
    },
    {
        title: "成員",
        path: 'users',
        loadComponent: () => import('./pages/users/users').then(m => m.Users)
    },
    // {
    //     title: "統計",
    //     path: 'statistics',
    //     loadComponent: () => import('./pages/statistics/statistics').then(m => m.Statistics)
    // },
    {
        title: "Project",
        path: 'project',
        loadComponent: () => import('./pages/project/project').then(m => m.Project)
    },

    // {
    //     title: "知識管理",
    //     path: 'knowledge-management',
    //     loadComponent: () => import('./pages/knowledge-management/knowledge-management').then(m => m.KnowledgeManagement)
    // },
    // {
    //     title: "角色管理",
    //     path: 'roles',
    //     loadComponent: () => import('./pages/roles/roles').then(m => m.Roles)
    // },
    {
        title: "qwertyuiop",
        path: 'qwertyuiop',
        loadComponent: () => import('./pages/reports/reports').then(m => m.Reports)
    },
    {
        title: "qwertyuiop2",
        path: 'qwertyuiop2',
        loadComponent: () => import('./pages/qwertyuiop2/qwertyuiop2').then(m => m.Qwertyuiop2)
    },
    {
        title: "工作大項",
        path: 'milestones',
        loadComponent: () => import('./pages/milestones/milestones').then(m => m.Milestones)
    },
    {
        title: "Pull Requests",
        path: 'pull-requests',
        loadComponent: () => import('./pages/prs/prs').then(m => m.Prs)
    },
    {
        title: "Commits",
        path: 'commits',
        loadComponent: () => import('./pages/commits/commits').then(m => m.Commits)
    },
    {
        title: "Repositories",
        path: 'repositories',
        loadComponent: () => import('./pages/repositories/repositories').then(m => m.Repositories)
    },
    {
        title: "需求確認歷史",
        path: 'requirement-confirmation-histories',
        loadComponent: () => import('./pages/requirement-confirmation-histories/requirement-confirmation-histories').then(m => m.RequirementConfirmationHistories)
    },
    {
        title: "需求確認清單",
        path: 'requirement-confirmation-lists',
        loadComponent: () => import('./pages/requirement-confirmation-requests/requirement-confirmation-requests').then(m => m.RequirementConfirmationRequests)
    },
    {
        title: "Plans",
        path: 'plans',
        loadComponent: () => import('./pages/plans/plans').then(m => m.Plans)
    },
];