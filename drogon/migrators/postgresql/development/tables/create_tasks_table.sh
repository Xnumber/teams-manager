#!/bin/bash
DB_NAME="$1"
if [ -z "$DB_NAME" ]; then
    echo "請提供要建立資料表的資料庫名稱作為參數。"
    exit 1
fi
echo -e "\033[1;36m建立工作資料表...\033[0m"
sudo -u postgres psql -d $DB_NAME <<EOF


CREATE TABLE IF NOT EXISTS "tasks" (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    creator_id UUID NOT NULL,
    creator_name VARCHAR(100) NOT NULL,
    tenant_id UUID,
    tenant_name VARCHAR(100),
    project_id UUID,
    project_name VARCHAR(100),
    task_type_id UUID,
    task_type_name VARCHAR(100),
    name VARCHAR(100) NOT NULL,
    excecutor_time_ratio FLOAT NOT NULL DEFAULT 0,
    description TEXT,
    concurrency_stamp VARCHAR(36) NOT NULL DEFAULT gen_random_uuid(),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    remark VARCHAR(2000),
    scheduled_start_date DATE NOT NULL,
    scheduled_end_date DATE NOT NULL,
    scheduled_completion_date DATE,
    completion_date DATE,
    progress DECIMAL(3,2) NOT NULL DEFAULT 0 CHECK (progress >= 0 AND progress <= 1),
    completed BOOLEAN NOT NULL DEFAULT FALSE,
    status_id UUID NOT NULL,
    status_name VARCHAR(100) NOT NULL,
    status_last_changed DATE NOT NULL DEFAULT CURRENT_DATE,
    result_description VARCHAR(500),
    team_id UUID,
    team_name VARCHAR(100),
    milestone_id UUID,
    milestone_name VARCHAR(100),
    estimated_working_days INTEGER NOT NULL DEFAULT 0,
    delayed BOOLEAN NOT NULL DEFAULT FALSE,

    
    CONSTRAINT fk_tenant
        FOREIGN KEY(tenant_id)
        REFERENCES tenants(id)
        ON DELETE NO ACTION,
    CONSTRAINT fk_project
        FOREIGN KEY(project_id)
        REFERENCES projects(id)
        ON DELETE NO ACTION,
    CONSTRAINT fk_task_type
        FOREIGN KEY(task_type_id)
        REFERENCES task_types(id)
        ON DELETE NO ACTION,
    CONSTRAINT fk_task_status
        FOREIGN KEY(status_id)
        REFERENCES task_status(id)
        ON DELETE SET NULL,
    CONSTRAINT fk_team
        FOREIGN KEY(team_id)
        REFERENCES teams(id)
        ON DELETE SET NULL,
    CONSTRAINT fk_milestone
        FOREIGN KEY(milestone_id)
        REFERENCES milestones(id)
        ON DELETE SET NULL
);
EOF