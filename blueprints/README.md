# Home Assistant blueprints

Reusable [Home Assistant automation blueprints](https://www.home-assistant.io/docs/automation/using_blueprints/)
for spas bridged with the Zavepower Energy Optimizer (see the repo
[README](../README.md)). They only reference entities you pick, so they work with
any device that exposes equivalent controls — nothing here is hard-wired to a
specific spa or device name.

| Blueprint | What it does |
|---|---|
| [`spa_time_sync.yaml`](automation/zavepower/spa_time_sync.yaml) | Presses the spa's **Sync Spa Time** button once a day to keep its internal clock aligned with Home Assistant. |
| [`spa_price_heating.yaml`](automation/zavepower/spa_price_heating.yaml) | Heats the spa hard when electricity is cheap and backs it off when it's expensive, on the days you choose. |

## Installing

### Import from URL (recommended)

In Home Assistant go to **Settings → Automations & Scenes → Blueprints →
Import Blueprint**, then paste the raw URL of the blueprint you want:

```
https://github.com/osro/esphome-zavepower/blob/main/blueprints/automation/zavepower/spa_time_sync.yaml
https://github.com/osro/esphome-zavepower/blob/main/blueprints/automation/zavepower/spa_price_heating.yaml
```

### Manual copy

Copy the `.yaml` file into your Home Assistant config under
`config/blueprints/automation/zavepower/`, then reload blueprints (or restart HA).

## Creating an automation

After importing, go to **Settings → Automations & Scenes → Create Automation →
Use Blueprint**, pick the blueprint, fill in the inputs, and save.

## Blueprints

### Spa – Daily Clock Sync (`spa_time_sync.yaml`)

The spa controller's clock drifts over time (and resets after a power cut),
which throws off filter-cycle scheduling. This presses the sync button daily.

| Input | Required | Default | Notes |
|---|---|---|---|
| Sync Spa Time button | yes | — | The spa's "Sync Spa Time" button entity. |
| Sync time | no | 12:00 | Time of day to run. |
| Only sync when connected | no | on | Skip the sync if the spa is unreachable. |
| Spa Connected sensor | no | — | Connectivity binary sensor, used only when the check above is on. |

### Spa – Spot-Price Heating Control (`spa_price_heating.yaml`)

When the price is "acceptable" on the selected days, the spa is put into
aggressive-heat mode (**High temperature range + Heat Mode Ready**); otherwise it
drops to eco (**Low range + Rest**). A short stagger separates the two switch
commands because the spa bus is slow half-duplex RS-485.

**Price source is generic** — it is not tied to Nord Pool. Either:

- point it at a **binary sensor** that is `on` when the price is acceptable
  (build that however you like — a threshold helper, template, cheapest-hours
  sensor, etc.), or
- leave that empty and pick a **numeric price sensor + threshold**; the blueprint
  decides acceptability itself.

It re-evaluates on a **timer** (default every 5 min), not on state changes — this
keeps it robust with optional inputs and handles the midnight day rollover. A hot
tub has plenty of thermal inertia, so the small latency is harmless.

| Input | Required | Default | Notes |
|---|---|---|---|
| Price-acceptable binary sensor | no* | — | Primary source. `on` = acceptable. |
| Price sensor (fallback) | no* | — | Numeric price, used only if no binary sensor set. |
| Price threshold (fallback) | no | 5 | In the price sensor's own units. |
| Acceptable when price is… | no | at or below | Which side of the threshold is acceptable. |
| Heat Mode (Ready) switch | yes | — | Ready/Rest switch. |
| High temperature range switch | no | — | High/Low range switch; leave empty if absent. |
| Stagger between commands | no | 10 s | Delay between the two switch commands. |
| Active days | no | every day | Days heating is allowed; other days stay eco. |
| Master enable toggle | no | — | `input_boolean`; when **off**, spa is forced to eco. |
| Spa-connected sensor | no | — | If set, does nothing while the spa is unreachable. |
| Re-evaluation interval | no | every 5 min | How often to re-check and adjust. |

\* Provide at least one price source. If neither is set, the price is treated as
not acceptable and the spa stays in eco (fail-safe).

> **Note:** turning the master enable toggle **off** actively backs the spa down
> to eco — it does not simply leave the spa alone.
