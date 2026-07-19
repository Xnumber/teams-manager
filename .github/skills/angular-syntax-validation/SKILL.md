---
name: angular-syntax-validation
description: "Validate and fix Angular TypeScript and template syntax issues using ngc. Use when Angular compile errors, template parser errors, TS diagnostics, build failures, or syntax warnings appear."
user-invocable: true
---

# Angular Syntax Validation

## Outcome
Produce a compile-clean Angular application by running compiler checks, fixing reported syntax issues, and re-validating until no blocking errors remain.

## When to Use
- Adjusting or refactoring Angular components, templates, or TypeScript code.
- Immediately after finishing any Angular-related implementation task.
- Angular compile fails during development or CI.
- Template parser errors appear in component HTML.
- TypeScript diagnostics block build output.
- You need a repeatable error-fix loop for syntax-level issues.

## Invocation
- Run `/angular-syntax-validation` after Angular code changes to validate compile health.

## 檢查使用指令
```bash
cd angular && npx ngc -p tsconfig.app.json --noEmit
```

## Procedure
1. Collect diagnostics and group by file, then by error code.
2. Fix errors first, then warnings that can become future failures.
3. Re-run the same compiler command after each fix batch.
4. Stop only when no blocking syntax/compile errors remain.
## Decision Logic
- If command fails with missing packages: run dependency install first, then retry.
- If errors are template-related: prioritize matching component class properties, bindings, and directive inputs/outputs.
- If errors are TS type-related: correct types and nullability instead of suppressing checks.
- If only warnings remain: evaluate impact, keep high-risk warnings in scope, defer low-risk warnings with notes.
## Quality Checks
- Compiler exits without errors on the final run.
- No new diagnostics are introduced in previously clean files.
- Fixes preserve intended runtime behavior.
- Any deferred warnings are explicitly documented with rationale.

Use this as a confidence pass for integration-level compile paths.