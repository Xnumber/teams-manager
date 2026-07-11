// PullRequestSimple.h
// Auto-generated struct for GitHub Pull Request Simple schema
// https://docs.github.com/en/rest/pulls/pulls?apiVersion=2026-03-10#list-pull-requests


#pragma once
#include <string>
#include <vector>
#include <optional>

struct SimpleUser {
    std::optional<std::string> name;
    std::optional<std::string> email;
    std::string login;
    int64_t id;
    std::string node_id;
    std::string avatar_url;
    std::optional<std::string> gravatar_id;
    std::string url;
    std::string html_url;
    std::string followers_url;
    std::string following_url;
    std::string gists_url;
    std::string starred_url;
    std::string subscriptions_url;
    std::string organizations_url;
    std::string repos_url;
    std::string events_url;
    std::string received_events_url;
    std::string type;
    bool site_admin;
    std::optional<std::string> starred_at;
    std::optional<std::string> user_view_type;
};

struct Label {
    int64_t id;
    std::string node_id;
    std::string url;
    std::string name;
    std::string description;
    std::string color;
    bool default_;
};

struct LicenseSimple {
    std::string key;
    std::string name;
    std::optional<std::string> url;
    std::optional<std::string> spdx_id;
    std::string node_id;
    std::string html_url;
};

struct RepositoryPermissions {
    bool admin;
    bool pull;
    bool push;
    std::optional<bool> triage;
    std::optional<bool> maintain;
};

struct Repository {
    int64_t id;
    std::string node_id;
    std::string name;
    std::string full_name;
    std::optional<LicenseSimple> license;
    int forks;
    RepositoryPermissions permissions;
    SimpleUser owner;
    bool private_;
    std::string html_url;
    std::optional<std::string> description;
    bool fork;
    std::string url;
    std::string archive_url;
    std::string assignees_url;
    std::string blobs_url;
    std::string branches_url;
    std::string collaborators_url;
    std::string comments_url;
    std::string commits_url;
    std::string compare_url;
    std::string contents_url;
    std::string contributors_url;
    std::string deployments_url;
    std::string downloads_url;
    std::string events_url;
    std::string forks_url;
    std::string git_commits_url;
    std::string git_refs_url;
    std::string git_tags_url;
    std::string hooks_url;
    std::string issue_comment_url;
    std::string issue_events_url;
    std::string issues_url;
    std::string keys_url;
    std::string labels_url;
    std::string languages_url;
    std::string merges_url;
    std::string milestones_url;
    std::string notifications_url;
    std::string pulls_url;
    std::string releases_url;
    std::string stargazers_url;
    std::string statuses_url;
    std::string subscribers_url;
    std::string subscription_url;
    std::string tags_url;
    std::string teams_url;
    std::string trees_url;
    std::string clone_url;
    std::optional<std::string> mirror_url;
    std::string hooks_url2;
    std::string svn_url;
    std::optional<std::string> homepage;
    std::optional<std::string> language;
    int forks_count;
    int stargazers_count;
    int watchers_count;
    int size;
    std::string default_branch;
    int open_issues_count;
    bool is_template;
    std::vector<std::string> topics;
    bool has_issues;
    bool has_projects;
    bool has_wiki;
    bool has_pages;
    bool has_discussions;
    bool has_pull_requests;
    std::optional<std::string> pull_request_creation_policy;
    bool archived;
    bool disabled;
    std::string visibility;
    std::optional<std::string> pushed_at;
    std::optional<std::string> created_at;
    std::optional<std::string> updated_at;
    bool allow_rebase_merge;
    std::string temp_clone_token;
    bool allow_squash_merge;
    bool allow_auto_merge;
    bool delete_branch_on_merge;
    bool allow_update_branch;
    std::optional<std::string> squash_merge_commit_title;
    std::optional<std::string> squash_merge_commit_message;
    std::optional<std::string> merge_commit_title;
    std::optional<std::string> merge_commit_message;
    bool allow_merge_commit;
    bool allow_forking;
    bool web_commit_signoff_required;
    int open_issues;
    int watchers;
    std::optional<std::string> starred_at;
    bool anonymous_access_enabled;
    // code_search_index_status omitted for brevity
};

struct HeadOrBase {
    std::string label;
    std::string ref;
    Repository repo;
    std::string sha;
    std::optional<SimpleUser> user;
};

struct Link {
    std::string href;
};

struct Links {
    Link comments;
    Link commits;
    Link statuses;
    Link html;
    Link issue;
    Link review_comments;
    Link review_comment;
    Link self;
};

struct PullRequestSimple {
    std::string url;
    int64_t id;
    std::string node_id;
    std::string html_url;
    std::string diff_url;
    std::string patch_url;
    std::string issue_url;
    std::string commits_url;
    std::string review_comments_url;
    std::string review_comment_url;
    std::string comments_url;
    std::string statuses_url;
    int number;
    std::string state;
    bool locked;
    std::string title;
    std::optional<SimpleUser> user;
    std::optional<std::string> body;
    std::vector<Label> labels;
    // milestone omitted for brevity
    std::optional<std::string> active_lock_reason;
    std::string created_at;
    std::string updated_at;
    std::optional<std::string> closed_at;
    std::optional<std::string> merged_at;
    std::vector<SimpleUser> assignees;
    std::vector<SimpleUser> requested_reviewers;
    // requested_teams omitted for brevity
    HeadOrBase head;
    HeadOrBase base;
    Links _links;
    std::string author_association;
    // auto_merge omitted for brevity
    bool draft;
};
