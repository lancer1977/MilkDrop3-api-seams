---
title: Mapped-Frontier Build Packet - MilkDrop3-api-seams finish line
status: draft
owner: lancer1977
issue: https://github.com/lancer1977/MilkDrop3-api-seams/issues/2
created: 2026-11-01
updated: 2026-11-01
---

# Mapped-Frontier Build Packet: MilkDrop3-api-seams finish line

## Intent

MilkDrop3 visualizer fork exposing a WM_COPYDATA API seam for stream automation. This packet records the repository's finish-line scope from open issues as of 2026-11-01: what remains, what blocks it, and the merge policy. Non-goals: no code, secret, auth, workflow or infrastructure change is made by this packet.

## Acceptance criteria

- [ ] [#2](https://github.com/lancer1977/MilkDrop3-api-seams/issues/2) Windows-runtime validation of WM_COPYDATA seam closed with evidence recorded on the issue.
- [ ] `.mfd/project.yaml` and the merge policy below stay in sync.

## Current behavior

- Integration branch: `main`; protected branch: `main`.
- Only the catalog-manifest workflow exists; no build or test CI. No dev branch, so PRs target main.

## Behavior map

Not applicable: repository-level finish-line packet. Feature behavior maps belong in per-issue packets.

## UI proof

Not applicable: docs-only packet.

## Technical contract

Unchanged. Any change to auth, secrets, workflows, deploy or infrastructure paths escalates to the owner (see `escalate_paths` in `.mfd/project.yaml`).

## Verification map

| Requirement | Scenario | Validation | Expected evidence |
| --- | --- | --- | --- |
| unit tier | PR to `main` | `None in CI` | Not in CI |
| integration tier | PR to `main` | `tools/send-milkdrop-copydata.ps1 against a running MilkDrop3 (manual, Windows)` | Not in CI |
| automation tier | PR to `main` | `Catalog Manifest workflow` | Catalog Manifest |

## Per-project merge policy

- Required checks: `Catalog Manifest`
- Integration branch: main
- Protected branch: main

| Test tier | Command | Required | CI job name |
| --- | --- | --- | --- |
| Unit | `None in CI` | No | None |
| Integration | `tools/send-milkdrop-copydata.ps1 against a running MilkDrop3 (manual, Windows)` | No | None |
| Automation | `Catalog Manifest workflow` | Yes | Catalog Manifest |

Checks observed in workflow files on `main`:

- `Catalog Manifest` from catalog-manifest.yml (reusable workflow on every pull_request; exact check-run name not confirmed)

## Delivery map

| Order | GitHub issue / card | Requirement / scenario | Dependencies | Validation |
| --- | --- | --- | --- | --- |
| 1 | [#2](https://github.com/lancer1977/MilkDrop3-api-seams/issues/2) | Windows-runtime validation of WM_COPYDATA seam | Needs a Windows runner (windows-latest hosted budget is exhausted portfolio-wide) | Required checks |

### Rollout and rollback

Docs-only. Roll back by reverting the packet commit. Deploys, if any, are out of scope and need separate owner approval.

### Blockers

| Blocker | Owner | Unblock condition |
| --- | --- | --- |
| [#2](https://github.com/lancer1977/MilkDrop3-api-seams/issues/2): Needs a Windows runner (windows-latest hosted budget is exhausted portfolio-wide) | Owner (infra) | Windows runner available or hosted budget approved |

### Future requirements

- Secrets: None
- Infrastructure: Windows CI runner
- Hardware: Windows machine with GPU for MilkDrop3

### Open decisions

| Decision | Options | Recommendation |
| --- | --- | --- |
| Create a dev integration branch? | Create dev / stay on main | Create dev to match portfolio policy (open) |

## Approval

- Product owner: lancer1977
- Decision: Pending
- Date: Pending
- Approved commit or immutable revision: Pending
- Approval evidence: Pending
- Approval brief: Pending

## Deviations

| Date | Requirement / boundary | Decision | Approver | Follow-up |
| --- | --- | --- | --- | --- |
| None | None | None | None | None |

## Release controls and closeout evidence

- Target branch: `main`
- Pull request: Pending
- Required reviewers: lancer1977
- CI and validation evidence: Pending

### Deployment contract

- Deployment applicability: Not applicable: docs-only packet; deployments need separate owner approval
- Target environments: None
- Deployment authority (separate from approval): lancer1977
- Source commit: Not applicable
- Immutable artifact / digest: Not applicable
- Rollout record: Not applicable
- Authenticated smoke proof: Not applicable
- Rollback target: Not applicable

### Release closeout record

- Live verification result: Not applicable
- Deferred drift / follow-up issue: None
