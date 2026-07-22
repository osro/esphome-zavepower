# Changelog

## [1.1.0](https://github.com/osro/esphome-zavepower/compare/v1.0.0...v1.1.0) (2026-07-22)


### Features

* Add component version text sensor and automated release workflow ([#115](https://github.com/osro/esphome-zavepower/issues/115)) ([ea99c8f](https://github.com/osro/esphome-zavepower/commit/ea99c8f226e186e2aa4293b7eaee6f8664336b76))
* Add Filter 2 Switch component for issue [#100](https://github.com/osro/esphome-zavepower/issues/100) ([#107](https://github.com/osro/esphome-zavepower/issues/107)) ([7c8563a](https://github.com/osro/esphome-zavepower/commit/7c8563af28998c3f7abb64ebff2b54f1f73486ee))
* add Home Assistant automation blueprints ([#1](https://github.com/osro/esphome-zavepower/issues/1)) ([219c298](https://github.com/osro/esphome-zavepower/commit/219c29827f7022a06deb26e0f83f3467a5a4b8cc))
* Add light domain support for spa lights ([#117](https://github.com/osro/esphome-zavepower/issues/117)) ([#119](https://github.com/osro/esphome-zavepower/issues/119)) ([ad70a23](https://github.com/osro/esphome-zavepower/commit/ad70a2371fd69a1735390ddd976552c8ebc1d9fd))
* add spa UART pin discovery firmware for Zavepower SmartBox ([cc9938f](https://github.com/osro/esphome-zavepower/commit/cc9938fcc350761480b865dca73d0d831f6f7c4b))
* add Status LED GPIO discovery firmware for Zavepower SmartBox ([cec5549](https://github.com/osro/esphome-zavepower/commit/cec55497f38a4a0f901e646b671c7487c5803f56))
* Add water_heater platform as alternative to climate ([#116](https://github.com/osro/esphome-zavepower/issues/116)) ([#120](https://github.com/osro/esphome-zavepower/issues/120)) ([c20e933](https://github.com/osro/esphome-zavepower/commit/c20e9330376e6fee72dcb620297704f6ceb40c78))
* add WiFi/API/OTA to discovery configs for network logs ([022b16d](https://github.com/osro/esphome-zavepower/commit/022b16d659eb9ff21aa81b57a9c8e7d7e08dff5d))
* add Zavepower SmartBox Balboa Spa example config with status LED ([318a4ed](https://github.com/osro/esphome-zavepower/commit/318a4ed52b1fb0691d9d11a3c758956a3f5fc435))
* enable native API encryption ([#4](https://github.com/osro/esphome-zavepower/issues/4)) ([74ec8b2](https://github.com/osro/esphome-zavepower/commit/74ec8b2110940ff77bc8fae9d2e34c3cf038753c))
* expose high-range and Ready/Rest heat controls ([307d3f6](https://github.com/osro/esphome-zavepower/commit/307d3f6ad77e05a37798b364f2a6c9cf1c9d4cb6))
* expose spa controls, diagnostics, and confirmed SmartBox pinout ([ecbd9b4](https://github.com/osro/esphome-zavepower/commit/ecbd9b4f6e274bc4a44b2fba2b48c2fe077bb856))
* expose spa diagnostics, fault log, and filter scheduling ([e98d845](https://github.com/osro/esphome-zavepower/commit/e98d8452c0fde55c3d23f988f67df358ba0c4a7d))
* publish as versioned ESPHome project with update notifications ([#8](https://github.com/osro/esphome-zavepower/issues/8)) ([2aca4b8](https://github.com/osro/esphome-zavepower/commit/2aca4b8dd961f74f85c428d17d129cf35a9a8641))
* set confirmed spa UART pins and drop conflicting LED roles ([15712d2](https://github.com/osro/esphome-zavepower/commit/15712d2a70d44085d3faaed4eb55d9e84e84c248))
* slow LED discovery blink window to ~10s per GPIO ([7131b09](https://github.com/osro/esphome-zavepower/commit/7131b09e5817948548828c5b7a73fc390199f9ec))
* wire confirmed LED pinout into SmartBox config ([47e5673](https://github.com/osro/esphome-zavepower/commit/47e5673f7fbf07965b690c155bb513b3bd56ba83))


### Bug Fixes

* add required mode to filter cycle text inputs ([97e597b](https://github.com/osro/esphome-zavepower/commit/97e597bbb8227c7fdeb452ed19ac7a7f17732331))
* correct RGB LED for common-anode wiring and pin mapping ([84749f3](https://github.com/osro/esphome-zavepower/commit/84749f3134fe9ec70f77cc3991f02be82bd4bc1f))
* decode spa temperature in Celsius to match panel ([42f349e](https://github.com/osro/esphome-zavepower/commit/42f349e8012299043267ba45c53c24cdbaaf85f7))
* Use correct constant names CLIMATE_SUPPORTS_* instead of CLIMATE_FEATURE_SUPPORTS_* ([9758cda](https://github.com/osro/esphome-zavepower/commit/9758cda82a01b3a1d7832c4fec4d8c2bb4487dae))
