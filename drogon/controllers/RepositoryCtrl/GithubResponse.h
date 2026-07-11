#pragma once
#include <string>
#include <vector>
#include <optional>

struct GithubUser {
	std::string login;
	int64_t id;
	std::string node_id;
	std::string avatar_url;
	std::string url;
	std::string html_url;
	std::string type;
	bool site_admin;
	// ...可依需求擴充
};

struct GithubPermissions {
	bool admin;
	bool maintain;
	bool push;
	bool triage;
	bool pull;
};

struct GithubResponse {
	int64_t id;
	std::string node_id;
	std::string name;
	std::string full_name;
	bool is_private;
	GithubUser owner;
	std::string html_url;
	std::optional<std::string> description;
	bool fork;
	std::string url;
	std::string created_at;
	std::string updated_at;
	std::string pushed_at;
	std::string git_url;
	std::string ssh_url;
	std::string clone_url;
	std::string svn_url;
	std::optional<std::string> homepage;
	int size;
	int stargazers_count;
	int watchers_count;
	std::string language;
	bool has_issues;
	bool has_projects;
	bool has_downloads;
	bool has_wiki;
	bool has_pages;
	bool has_discussions;
	int forks_count;
	std::optional<std::string> mirror_url;
	bool archived;
	bool disabled;
	int open_issues_count;
	std::optional<std::string> license;
	bool allow_forking;
	bool is_template;
	bool web_commit_signoff_required;
	bool has_pull_requests;
	std::string pull_request_creation_policy;
	std::vector<std::string> topics;
	std::string visibility;
	int forks;
	int open_issues;
	int watchers;
	std::string default_branch;
	GithubPermissions permissions;
	GithubUser organization;
	int network_count;
	int subscribers_count;
	// ...可依需求擴充
};
