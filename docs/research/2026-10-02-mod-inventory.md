# NML Workshop candidate inventory — 2026-10-02

**Research snapshot, not an approved modpack.** This records 87 unique candidate/download packages: 68 current candidate entries and 19 additional dependency packages. It does not promote repository candidates, register mods, or authorize implementation.

Read the [research summary](2026-10-01-mod-candidate-review.md) and [arsenal/resource handoff](2026-10-02-arsenal-status.md) alongside this inventory. [AI operating rules](../AI_OPERATING_RULES.md), [workboard](../AI_WORKBOARD.md), and accepted ADRs remain authoritative.

## Evidence and status

- **Member-selected:** explicitly preferred by the member for evaluation. This is not Weston approval.
- **Retained candidate:** kept for inspection/comparison; final inclusion remains open.
- **Required dependency:** supports retained content; include only if that content is approved.
- **Installed version:** version recorded in local Workshop metadata, not a guarantee of the newest available release.
- **Local checks:** 86/87 current packages have matching project GUIDs, required project/data/resource-database files and matching local manifest sizes. Discord Admin Stats is missing. These checks do not verify hashes, successful Workbench loading, gameplay or performance.
- **Gameplay:** Codex has not independently gameplay-tested this set. VT4 driving and CERTII + Best Body Cam working together are member reports, detailed below. Every other package has no documented gameplay result and working status is unknown.
- **Compatibility notes:** author claims and role overlaps are research evidence, not observed failures unless explicitly identified.
- **Sources:** existing local organized/explained research and download audit dated 2026-10-02, local Workshop project files and the supplied earlier arsenal handoff. Original Workshop research was checked 2026-10-01, with MCR, SCAR, Best Body Cam and selected descriptions refreshed 2026-10-02. This publication is not a fresh review of all Workshop pages.

## Dependency interpretation

All installed current-list project GUIDs equal the Workshop IDs shown in their entries. Vanilla Reforger is `58D0FB3206B6F859` and is omitted from external dependency lists. Empty dependency strings are ignored.

Direct relationships below come from the installed `.gproj` files; additional transitive prerequisites are computed from those direct relationships. For the missing Discord package, dependencies remain the earlier published-page record and its project GUID is unverified.

The earlier Workshop-page graph sometimes presented the full prerequisite closure as direct dependencies. That changes the relationship labels, not the list of required packages. The installed newer Minnesinger Headwear project additionally lists NV-System directly; NV-System and its prerequisites are already present in the 87-package set.

## Quick inventory

| Package | Workshop ID | Installed version | Member status | Local files |
|---|---|---|---|---|
| [Zeliks Character](https://reforger.armaplatform.com/workshop/5D0551624969C92E) | `5D0551624969C92E` | 1.1.21 | Member-selected | Checked |
| [Attachment Framework-Core](https://reforger.armaplatform.com/workshop/687CD82F6E41D627) | `687CD82F6E41D627` | 1.0.16 | Required dependency | Checked |
| [Attachment Framework](https://reforger.armaplatform.com/workshop/645F08FA9E7CDEDE) | `645F08FA9E7CDEDE` | 1.0.42 | Required dependency | Checked |
| [NV-System](https://reforger.armaplatform.com/workshop/64722DADC53CB75E) | `64722DADC53CB75E` | 1.0.35 | Required dependency | Checked |
| [FORTEX Framework](https://reforger.armaplatform.com/workshop/66C2ED22F25B05C9) | `66C2ED22F25B05C9` | 1.0.45 | Required dependency | Checked |
| [RTS Protocol](https://reforger.armaplatform.com/workshop/670E255D949409E3) | `670E255D949409E3` | 0.0.37 | Required dependency | Checked |
| [RHS — Content Pack 01](https://reforger.armaplatform.com/workshop/1337C0DE5DABBEEF) | `1337C0DE5DABBEEF` | 0.16.5208 | Required dependency | Checked |
| [RHS — Content Pack 02](https://reforger.armaplatform.com/workshop/BADC0DEDABBEDA5E) | `BADC0DEDABBEDA5E` | 0.16.5208 | Required dependency | Checked |
| [RHS — Status Quo](https://reforger.armaplatform.com/workshop/595F2BF2F44836FB) | `595F2BF2F44836FB` | 0.16.5208 | Required dependency | Checked |
| [GRS — Dev Framework](https://reforger.armaplatform.com/workshop/65DACC64CE785B6C) | `65DACC64CE785B6C` | 1.0.52 | Required dependency | Checked |
| [Ukraine Armed Forces — Red Thunder System](https://reforger.armaplatform.com/workshop/67126A42BAE3B5EB) | `67126A42BAE3B5EB` | 0.0.22 | Retained candidate | Checked |
| [Ukraine Armed Forces RHS](https://reforger.armaplatform.com/workshop/65F92D51845AC237) | `65F92D51845AC237` | 1.0.8 | Retained candidate | Checked |
| [FORTEX — Russian Tactical Gear](https://reforger.armaplatform.com/workshop/647CB046E0EDE0D9) | `647CB046E0EDE0D9` | 1.0.79 | Retained candidate | Checked |
| [FORTEX — UTG](https://reforger.armaplatform.com/workshop/69293A8ED08A262D) | `69293A8ED08A262D` | 1.1.6 | Retained candidate | Checked |
| [Minnesinger Clothes](https://reforger.armaplatform.com/workshop/6A6509AC32B5A49A) | `6A6509AC32B5A49A` | 0.9.9 | Member-selected | Checked |
| [Minnesinger Core](https://reforger.armaplatform.com/workshop/6A6507D034ECFC69) | `6A6507D034ECFC69` | 1.0.1 | Member-selected | Checked |
| [Minnesinger Gear](https://reforger.armaplatform.com/workshop/6A65091824BEC7DF) | `6A65091824BEC7DF` | 0.9.9 | Member-selected | Checked |
| [Minnesinger Headwear](https://reforger.armaplatform.com/workshop/6A65098A37A59C06) | `6A65098A37A59C06` | 0.9.94 | Member-selected | Checked |
| [MNSGR Ghillies — STANDALONE](https://reforger.armaplatform.com/workshop/6A3B3A10920F7DBA) | `6A3B3A10920F7DBA` | 1.0.0 | Retained candidate | Checked |
| [GRS_BEARDS (formerly listed as CHAMPS_BEARDS)](https://reforger.armaplatform.com/workshop/68AB562502EB08A3) | `68AB562502EB08A3` | 1.0.7 | Retained candidate | Checked |
| [Flag Patches — Country](https://reforger.armaplatform.com/workshop/78C24C775ECE66C3) | `78C24C775ECE66C3` | 2.0.0 | Retained candidate | Checked |
| [GRS — Patches](https://reforger.armaplatform.com/workshop/657B064AE0E231DF) | `657B064AE0E231DF` | 1.0.26 | Required dependency | Checked |
| [SlavicWar Caryable Flags](https://reforger.armaplatform.com/workshop/6A6B2DB5E4EFDCDF) | `6A6B2DB5E4EFDCDF` | 1.0.0 | Retained candidate | Checked |
| [Caryable Flags](https://reforger.armaplatform.com/workshop/62BF3C345D95EF63) | `62BF3C345D95EF63` | 1.0.0 | Required dependency | Checked |
| [Chungus Shotguns](https://reforger.armaplatform.com/workshop/620E584B1D2C96A4) | `620E584B1D2C96A4` | 1.0.43 | Retained candidate | Checked |
| [RGM-40_GL-06](https://reforger.armaplatform.com/workshop/69148A1D52696666) | `69148A1D52696666` | 1.0.9 | Retained candidate | Checked |
| [GRS Weapons 2](https://reforger.armaplatform.com/workshop/E4D206B81482F59E) | `E4D206B81482F59E` | 1.0.20 | Retained candidate | Checked |
| [HK-G95KA1](https://reforger.armaplatform.com/workshop/6A3331DC109F8B6F) | `6A3331DC109F8B6F` | 1.0.0 | Retained candidate | Checked |
| [MSBS Grot](https://reforger.armaplatform.com/workshop/6590ECF80DD17257) | `6590ECF80DD17257` | 0.0.40 | Retained candidate | Checked |
| [MCR-Pack 5.56x45](https://reforger.armaplatform.com/workshop/5D7531C2D05BC700) | `5D7531C2D05BC700` | 1.0.11 | Member-selected | Checked |
| [FN SCAR — Heavy and Light](https://reforger.armaplatform.com/workshop/616478A18DC7DCB6) | `616478A18DC7DCB6` | 1.6.7 | Member-selected | Checked |
| [FORTEX RTW](https://reforger.armaplatform.com/workshop/68F92DCEEC950E1C) | `68F92DCEEC950E1C` | 1.0.28 | Required dependency | Checked |
| [FORTEX — RU Weapon Core](https://reforger.armaplatform.com/workshop/690AC3895BAAFAE1) | `690AC3895BAAFAE1` | 1.0.91 | Required dependency | Checked |
| [Extended Fortex Taped Mags](https://reforger.armaplatform.com/workshop/6A3B4D970AC5EC13) | `6A3B4D970AC5EC13` | 1.0.1 | Retained candidate | Checked |
| [AK74 Retextures](https://reforger.armaplatform.com/workshop/6A5D18F82B3210CD) | `6A5D18F82B3210CD` | 1.0.10 | Retained candidate | Checked |
| [Bacon Suppressors](https://reforger.armaplatform.com/workshop/5AB301290317994A) | `5AB301290317994A` | 1.2.7 | Retained candidate | Checked |
| [GRS Suppressor Pack](https://reforger.armaplatform.com/workshop/6A30C1606411936A) | `6A30C1606411936A` | 1.0.2 | Required dependency | Checked |
| [RIS Laser Attachments](https://reforger.armaplatform.com/workshop/5ABD0CB57F7E9EB1) | `5ABD0CB57F7E9EB1` | 1.8.0 | Required dependency | Checked |
| [SW-Reap-IR thermal scope](https://reforger.armaplatform.com/workshop/695A2C3D78F0B612) | `695A2C3D78F0B612` | 1.0.24 | Retained candidate | Checked |
| [VooDoo-S Thermal Clip-On](https://reforger.armaplatform.com/workshop/CEF13D07B83D0A23) | `CEF13D07B83D0A23` | 1.0.1 | Retained candidate | Checked |
| [Laser Rangefinder](https://reforger.armaplatform.com/workshop/620E426C34BE0D17) | `620E426C34BE0D17` | 1.0.12 | Required dependency | Checked |
| [Ukraine Armed Forces RHS Fix](https://reforger.armaplatform.com/workshop/673C00E41ACDC2D4) | `673C00E41ACDC2D4` | 1.0.16 | Required dependency | Checked |
| [Extra Grenades](https://reforger.armaplatform.com/workshop/66B500DF95F2C3AF) | `66B500DF95F2C3AF` | 1.2.7 | Member-selected | Checked |
| [Extra Impact Grenades](https://reforger.armaplatform.com/workshop/68A0F788C81285BF) | `68A0F788C81285BF` | 1.0.6 | Member-selected | Checked |
| [Extra Mines](https://reforger.armaplatform.com/workshop/66C75538DD0251D1) | `66C75538DD0251D1` | 1.1.0 | Member-selected | Checked |
| [Mark 1 Thermobaric Grenade](https://reforger.armaplatform.com/workshop/68E53DBBFC8EFC78) | `68E53DBBFC8EFC78` | 1.0.6 | Retained candidate | Checked |
| [RDG-2 Colored Smoke Grenades](https://reforger.armaplatform.com/workshop/6A5E08CF942970BB) | `6A5E08CF942970BB` | 1.0.3 | Retained candidate | Checked |
| [Ukrainian War Vehicles](https://reforger.armaplatform.com/workshop/699E29EBC9B02967) | `699E29EBC9B02967` | 0.0.7 | Retained candidate | Checked |
| [RUVicExpansion](https://reforger.armaplatform.com/workshop/6A295E0AF0B307E7) | `6A295E0AF0B307E7` | 1.0.8 | Retained candidate | Checked |
| [VT4 — FRM](https://reforger.armaplatform.com/workshop/663B2784961621FB) | `663B2784961621FB` | 1.0.11 | Retained candidate | Checked |
| [M1224 Maxxpro MRAP 1.8 Fix](https://reforger.armaplatform.com/workshop/6A5169730316933C) | `6A5169730316933C` | 1.0.2 | Retained candidate | Checked |
| [M1224 Maxxpro MRAP](https://reforger.armaplatform.com/workshop/684D34D51DC5E22A) | `684D34D51DC5E22A` | 0.0.8 | Required dependency | Checked |
| [PR_UTILS](https://reforger.armaplatform.com/workshop/686104581D2D722B) | `686104581D2D722B` | 0.0.3 | Required dependency | Checked |
| [Realistic Combat Drones](https://reforger.armaplatform.com/workshop/65AD60E204191D37) | `65AD60E204191D37` | 2.3.7 | Retained candidate | Checked |
| [ACE Core Dev](https://reforger.armaplatform.com/workshop/65AD7D0D9941A380) | `65AD7D0D9941A380` | 1.5.39 | Member-selected | Checked |
| [ACE Medical Core Dev](https://reforger.armaplatform.com/workshop/6586079789278413) | `6586079789278413` | 1.5.36 | Member-selected | Checked |
| [ACE Medical Hitzones Dev](https://reforger.armaplatform.com/workshop/65B343F799FB521B) | `65B343F799FB521B` | 1.5.36 | Member-selected | Checked |
| [ACE Medical Circulation Dev](https://reforger.armaplatform.com/workshop/65AD7D4F994EA327) | `65AD7D4F994EA327` | 1.5.36 | Member-selected | Checked |
| [ACE Medical Breathing Dev](https://reforger.armaplatform.com/workshop/671F73D99978B4F2) | `671F73D99978B4F2` | 1.5.36 | Member-selected | Checked |
| [SplintACE](https://reforger.armaplatform.com/workshop/697CC096F5E423E0) | `697CC096F5E423E0` | 1.0.7 | Member-selected | Checked |
| [CERTII-SimulatedMovement](https://reforger.armaplatform.com/workshop/69B252D91D8172C2) | `69B252D91D8172C2` | 7.0.10 | Member-selected | Checked |
| [Best Body Cam](https://reforger.armaplatform.com/workshop/64FD308B60F7B687) | `64FD308B60F7B687` | 1.0.9 | Member-selected | Checked |
| [improved reforger ragdolls](https://reforger.armaplatform.com/workshop/64594B4F6C4E9718) | `64594B4F6C4E9718` | 1.0.7 | Retained candidate | Checked |
| [TGZ_RecoilRealism](https://reforger.armaplatform.com/workshop/699923D8F24A15D0) | `699923D8F24A15D0` | 1.0.0 | Retained candidate | Checked |
| [Keep Gun When Uncon](https://reforger.armaplatform.com/workshop/6088A3044B7ECBFD) | `6088A3044B7ECBFD` | 1.0.1 | Retained candidate | Checked |
| [GC Suppression](https://reforger.armaplatform.com/workshop/684CE8AA3B1D6573) | `684CE8AA3B1D6573` | 1.7.2 | Member-selected | Checked |
| [2-7 Suppressor Overheating](https://reforger.armaplatform.com/workshop/66B073D763F66862) | `66B073D763F66862` | 1.1.7 | Member-selected | Checked |
| [Improved Blood Effect Deluxe](https://reforger.armaplatform.com/workshop/660896EB172D4B7F) | `660896EB172D4B7F` | 1.0.3 | Member-selected | Checked |
| [Improved Blood Effect](https://reforger.armaplatform.com/workshop/62FCEB51DF8527B6) | `62FCEB51DF8527B6` | 1.5.9 | Member-selected | Checked |
| [Ukraine Radio Chatter](https://reforger.armaplatform.com/workshop/6532B17102092B01) | `6532B17102092B01` | 0.0.1 | Retained candidate | Checked |
| [Realistic Combat Sound Mod](https://reforger.armaplatform.com/workshop/68C6D7DD75DBDB57) | `68C6D7DD75DBDB57` | 0.5.8 | Member-selected | Checked |
| [Screams of War](https://reforger.armaplatform.com/workshop/69E67B11DA454B9E) | `69E67B11DA454B9E` | 1.0.3 | Member-selected | Checked |
| [LG_EarPlugs](https://reforger.armaplatform.com/workshop/6A14E4F10EA40001) | `6A14E4F10EA40001` | 1.0.2 | Retained candidate | Checked |
| [Player Map Markers](https://reforger.armaplatform.com/workshop/5E92F5A4A1B75A75) | `5E92F5A4A1B75A75` | 1.3.2 | Retained candidate | Checked |
| [Where Am I](https://reforger.armaplatform.com/workshop/5965550F24A0C152) | `5965550F24A0C152` | 1.2.0 | Retained candidate | Checked |
| [Kova x ATAK](https://reforger.armaplatform.com/workshop/6A49417788E13618) | `6A49417788E13618` | 0.0.8 | Required dependency | Checked |
| [ATAK x COMPAT](https://reforger.armaplatform.com/workshop/6A756C77B915750B) | `6A756C77B915750B` | 1.0.0 | Retained candidate | Checked |
| [Minnesinger X ATAK Compat](https://reforger.armaplatform.com/workshop/6A7567D790CA7DBF) | `6A7567D790CA7DBF` | 1.0.0 | Retained candidate | Checked |
| [Bacon Loadout Editor](https://reforger.armaplatform.com/workshop/606B100247F5C709) | `606B100247F5C709` | 1.7.3 | Retained candidate | Checked |
| [FTA Persistent Ranks](https://reforger.armaplatform.com/workshop/68A01393E2138094) | `68A01393E2138094` | 1.0.28 | Member-selected | Checked |
| [CERTII-Minimal-HUD](https://reforger.armaplatform.com/workshop/69CEF7D584AFD1FA) | `69CEF7D584AFD1FA` | 7.0.8 | Member-selected | Checked |
| [CERTII-Grenade-Limiter](https://reforger.armaplatform.com/workshop/69D92CDA0AE467E1) | `69D92CDA0AE467E1` | 7.0.2 | Member-selected | Checked |
| [Voice Volume Slider](https://reforger.armaplatform.com/workshop/647C19ACB69E7914) | `647C19ACB69E7914` | 1.0.22 | Retained candidate | Checked |
| [WCS_SpawnProtection](https://reforger.armaplatform.com/workshop/614C00DA7F8765F6) | `614C00DA7F8765F6` | 8.1.0 | Retained candidate | Checked |
| [WCS_VehicleLock](https://reforger.armaplatform.com/workshop/61BA4EB5C886D396) | `61BA4EB5C886D396` | 8.1.0 | Retained candidate | Checked |
| [GM Tools](https://reforger.armaplatform.com/workshop/64F10E068D5880A6) | `64F10E068D5880A6` | 2.2.0 | Retained candidate | Checked |
| [Discord Admin Stats](https://reforger.armaplatform.com/workshop/68C61B955A988F7E) | `68C61B955A988F7E` | Unknown | Retained candidate | **Missing** |

## Package details

### 1. Shared foundations and frameworks

#### [Zeliks Character](https://reforger.armaplatform.com/workshop/5D0551624969C92E)

- **Workshop ID:** `5D0551624969C92E`. **Project GUID:** `5D0551624969C92E` (locally checked).
- **Installed version:** 1.1.21. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** member-selected.
- **Purpose:** Character framework used by compatible clothing and appearance packs; it is not itself a face-selection menu.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Retain for controlled testing as the member preference; production approval remains with Weston.
- **Problems / compatibility:** Selected prerequisite for newer Minnesinger packs.

#### [Attachment Framework-Core](https://reforger.armaplatform.com/workshop/687CD82F6E41D627)

- **Workshop ID:** `687CD82F6E41D627`. **Project GUID:** `687CD82F6E41D627` (locally checked).
- **Installed version:** 1.0.16. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** required dependency.
- **Purpose:** Scripts and essential configuration for custom attachments, modular helmets and the Attachment Framework systems.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Retain only if its parent content is approved; no production registration recommended by this research.
- **Problems / compatibility:** No individual failure established by this research. Full-stack behavior remains untested.

#### [Attachment Framework](https://reforger.armaplatform.com/workshop/645F08FA9E7CDEDE)

- **Workshop ID:** `645F08FA9E7CDEDE`. **Project GUID:** `645F08FA9E7CDEDE` (locally checked).
- **Installed version:** 1.0.42. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** required dependency.
- **Purpose:** Attachment Framework asset pack and examples for modular attachments, including flashlights and lasers.
- **Direct external dependencies:** [Attachment Framework-Core](https://reforger.armaplatform.com/workshop/687CD82F6E41D627) (`687CD82F6E41D627`)
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Retain only if its parent content is approved; no production registration recommended by this research.
- **Problems / compatibility:** No individual failure established by this research. Full-stack behavior remains untested.

#### [NV-System](https://reforger.armaplatform.com/workshop/64722DADC53CB75E)

- **Workshop ID:** `64722DADC53CB75E`. **Project GUID:** `64722DADC53CB75E` (locally checked).
- **Installed version:** 1.0.35. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** required dependency.
- **Purpose:** Scripted night vision with IR support, helmet/headstrap slots and white/green phosphor modes; includes GPNVG-18 equipment.
- **Direct external dependencies:** [Attachment Framework](https://reforger.armaplatform.com/workshop/645F08FA9E7CDEDE) (`645F08FA9E7CDEDE`)
- **Additional transitive dependencies:** [Attachment Framework-Core](https://reforger.armaplatform.com/workshop/687CD82F6E41D627) (`687CD82F6E41D627`)
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Retain only if its parent content is approved; no production registration recommended by this research.
- **Problems / compatibility:** No individual failure established by this research. Full-stack behavior remains untested.

#### [FORTEX Framework](https://reforger.armaplatform.com/workshop/66C2ED22F25B05C9)

- **Workshop ID:** `66C2ED22F25B05C9`. **Project GUID:** `66C2ED22F25B05C9` (locally checked).
- **Installed version:** 1.0.45. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** required dependency.
- **Purpose:** Core scripts and support components for FORTEX mods. The author says it has no standalone gameplay content.
- **Direct external dependencies:** [NV-System](https://reforger.armaplatform.com/workshop/64722DADC53CB75E) (`64722DADC53CB75E`)
- **Additional transitive dependencies:** [Attachment Framework](https://reforger.armaplatform.com/workshop/645F08FA9E7CDEDE) (`645F08FA9E7CDEDE`); [Attachment Framework-Core](https://reforger.armaplatform.com/workshop/687CD82F6E41D627) (`687CD82F6E41D627`)
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Retain only if its parent content is approved; no production registration recommended by this research.
- **Problems / compatibility:** No individual failure established by this research. Full-stack behavior remains untested.

#### [RTS Protocol](https://reforger.armaplatform.com/workshop/670E255D949409E3)

- **Workshop ID:** `670E255D949409E3`. **Project GUID:** `670E255D949409E3` (locally checked).
- **Installed version:** 0.0.37. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** required dependency.
- **Purpose:** RTS framework adding modular clothing/equipment slots for vests, helmets, shirts and related gear.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Retain only if its parent content is approved; no production registration recommended by this research.
- **Problems / compatibility:** The published license says public servers/communities require prior written authorization. This is a page-specific requirement to resolve before public deployment, not a blanket claim about all mods.

#### [RHS — Content Pack 01](https://reforger.armaplatform.com/workshop/1337C0DE5DABBEEF)

- **Workshop ID:** `1337C0DE5DABBEEF`. **Project GUID:** `1337C0DE5DABBEEF` (locally checked).
- **Installed version:** 0.16.5208. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** required dependency.
- **Purpose:** RHS supporting content package. Its Workshop description does not itemize the precise contents of this individual pack.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Retain only if its parent content is approved; no production registration recommended by this research.
- **Problems / compatibility:** No individual failure established by this research. Full-stack behavior remains untested.

#### [RHS — Content Pack 02](https://reforger.armaplatform.com/workshop/BADC0DEDABBEDA5E)

- **Workshop ID:** `BADC0DEDABBEDA5E`. **Project GUID:** `BADC0DEDABBEDA5E` (locally checked).
- **Installed version:** 0.16.5208. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** required dependency.
- **Purpose:** Second RHS supporting content package. Its Workshop description does not itemize the precise contents of this individual pack.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Retain only if its parent content is approved; no production registration recommended by this research.
- **Problems / compatibility:** No individual failure established by this research. Full-stack behavior remains untested.

#### [RHS — Status Quo](https://reforger.armaplatform.com/workshop/595F2BF2F44836FB)

- **Workshop ID:** `595F2BF2F44836FB`. **Project GUID:** `595F2BF2F44836FB` (locally checked).
- **Installed version:** 0.16.5208. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** required dependency.
- **Purpose:** RHS contemporary military package centered on US and Russian Federation equipment; brings its own content and systems.
- **Direct external dependencies:** [RHS — Content Pack 01](https://reforger.armaplatform.com/workshop/1337C0DE5DABBEEF) (`1337C0DE5DABBEEF`); [RHS — Content Pack 02](https://reforger.armaplatform.com/workshop/BADC0DEDABBEDA5E) (`BADC0DEDABBEDA5E`)
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Retain only if its parent content is approved; no production registration recommended by this research.
- **Problems / compatibility:** No individual failure established by this research. Full-stack behavior remains untested.

#### [GRS — Dev Framework](https://reforger.armaplatform.com/workshop/65DACC64CE785B6C)

- **Workshop ID:** `65DACC64CE785B6C`. **Project GUID:** `65DACC64CE785B6C` (locally checked).
- **Installed version:** 1.0.52. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** required dependency.
- **Purpose:** GRS shared development framework in the country-patch dependency chain; exact standalone features are not established in this review.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Retain only if its parent content is approved; no production registration recommended by this research.
- **Problems / compatibility:** No individual failure established by this research. Full-stack behavior remains untested.

### 2. Faction clothing, equipment and appearance

#### [Ukraine Armed Forces — Red Thunder System](https://reforger.armaplatform.com/workshop/67126A42BAE3B5EB)

- **Workshop ID:** `67126A42BAE3B5EB`. **Project GUID:** `67126A42BAE3B5EB` (locally checked).
- **Installed version:** 0.0.22. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** retained candidate.
- **Purpose:** RTS Ukrainian clothing/equipment candidate, including balaclavas, G3/G4 jackets and MTAC softshells.
- **Direct external dependencies:** [RTS Protocol](https://reforger.armaplatform.com/workshop/670E255D949409E3) (`670E255D949409E3`)
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Inspect and test before deciding on production inclusion.
- **Problems / compatibility:** User retained for content comparison with RHS pack: equipment/uniform pack including balaclavas, G3/G4 jackets and MTAC softshells; requires RTS Protocol. Combined compatibility untested.

#### [Ukraine Armed Forces RHS](https://reforger.armaplatform.com/workshop/65F92D51845AC237)

- **Workshop ID:** `65F92D51845AC237`. **Project GUID:** `65F92D51845AC237` (locally checked).
- **Installed version:** 1.0.8. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** retained candidate.
- **Purpose:** Ukrainian uniform/helmet retextures, patches and loadout additions built around RHS and vanilla US.
- **Direct external dependencies:** [RHS — Status Quo](https://reforger.armaplatform.com/workshop/595F2BF2F44836FB) (`595F2BF2F44836FB`); [RHS — Content Pack 01](https://reforger.armaplatform.com/workshop/1337C0DE5DABBEEF) (`1337C0DE5DABBEEF`); [RHS — Content Pack 02](https://reforger.armaplatform.com/workshop/BADC0DEDABBEDA5E) (`BADC0DEDABBEDA5E`)
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Inspect and test before deciding on production inclusion.
- **Problems / compatibility:** User retained for content comparison with RTS pack. Adds Ukrainian uniform/helmet retextures, patches and loadout to vanilla US; requires RHS packs01/02 and Status Quo. Also required by supplied SW-Reap-IR scope. Combined compatibility untested.

#### [FORTEX — Russian Tactical Gear](https://reforger.armaplatform.com/workshop/647CB046E0EDE0D9)

- **Workshop ID:** `647CB046E0EDE0D9`. **Project GUID:** `647CB046E0EDE0D9` (locally checked).
- **Installed version:** 1.0.79. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** retained candidate.
- **Purpose:** Russian tactical clothing and equipment from FORTEX.
- **Direct external dependencies:** [Zeliks Character](https://reforger.armaplatform.com/workshop/5D0551624969C92E) (`5D0551624969C92E`); [FORTEX Framework](https://reforger.armaplatform.com/workshop/66C2ED22F25B05C9) (`66C2ED22F25B05C9`)
- **Additional transitive dependencies:** [NV-System](https://reforger.armaplatform.com/workshop/64722DADC53CB75E) (`64722DADC53CB75E`); [Attachment Framework](https://reforger.armaplatform.com/workshop/645F08FA9E7CDEDE) (`645F08FA9E7CDEDE`); [Attachment Framework-Core](https://reforger.armaplatform.com/workshop/687CD82F6E41D627) (`687CD82F6E41D627`)
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Inspect and test before deciding on production inclusion.
- **Problems / compatibility:** Retained; Russian equipment.

#### [FORTEX — UTG](https://reforger.armaplatform.com/workshop/69293A8ED08A262D)

- **Workshop ID:** `69293A8ED08A262D`. **Project GUID:** `69293A8ED08A262D` (locally checked).
- **Installed version:** 1.1.6. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** retained candidate.
- **Purpose:** FORTEX Ukrainian tactical gear; this is the package supplied as “Cortex uki.”
- **Direct external dependencies:** [FORTEX Framework](https://reforger.armaplatform.com/workshop/66C2ED22F25B05C9) (`66C2ED22F25B05C9`); [Zeliks Character](https://reforger.armaplatform.com/workshop/5D0551624969C92E) (`5D0551624969C92E`); [Attachment Framework](https://reforger.armaplatform.com/workshop/645F08FA9E7CDEDE) (`645F08FA9E7CDEDE`); [Attachment Framework-Core](https://reforger.armaplatform.com/workshop/687CD82F6E41D627) (`687CD82F6E41D627`); [NV-System](https://reforger.armaplatform.com/workshop/64722DADC53CB75E) (`64722DADC53CB75E`)
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Inspect and test before deciding on production inclusion.
- **Problems / compatibility:** Retained; Ukrainian tactical gear. This is the supplied “Cortex uki” link. Requires Zeliks, Attachment Framework/Core, NV-System and FORTEX Framework.

#### [Minnesinger Clothes](https://reforger.armaplatform.com/workshop/6A6509AC32B5A49A)

- **Workshop ID:** `6A6509AC32B5A49A`. **Project GUID:** `6A6509AC32B5A49A` (locally checked).
- **Installed version:** 0.9.9. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** member-selected.
- **Purpose:** Newer Minnesinger clothing pack selected during the review.
- **Direct external dependencies:** [Zeliks Character](https://reforger.armaplatform.com/workshop/5D0551624969C92E) (`5D0551624969C92E`)
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Retain for controlled testing as the member preference; production approval remains with Weston.
- **Problems / compatibility:** Selected newer family; requires Zeliks.

#### [Minnesinger Core](https://reforger.armaplatform.com/workshop/6A6507D034ECFC69)

- **Workshop ID:** `6A6507D034ECFC69`. **Project GUID:** `6A6507D034ECFC69` (locally checked).
- **Installed version:** 1.0.1. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** member-selected.
- **Purpose:** Newer Minnesinger base/core package used by that family, including Headwear. Exact standalone item contents remain to be inspected.
- **Direct external dependencies:** [Zeliks Character](https://reforger.armaplatform.com/workshop/5D0551624969C92E) (`5D0551624969C92E`)
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Retain for controlled testing as the member preference; production approval remains with Weston.
- **Problems / compatibility:** Selected newer family; requires Zeliks.

#### [Minnesinger Gear](https://reforger.armaplatform.com/workshop/6A65091824BEC7DF)

- **Workshop ID:** `6A65091824BEC7DF`. **Project GUID:** `6A65091824BEC7DF` (locally checked).
- **Installed version:** 0.9.9. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** member-selected.
- **Purpose:** Newer Minnesinger equipment/gear pack selected during the review.
- **Direct external dependencies:** [Zeliks Character](https://reforger.armaplatform.com/workshop/5D0551624969C92E) (`5D0551624969C92E`)
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Retain for controlled testing as the member preference; production approval remains with Weston.
- **Problems / compatibility:** Selected newer family; requires Zeliks.

#### [Minnesinger Headwear](https://reforger.armaplatform.com/workshop/6A65098A37A59C06)

- **Workshop ID:** `6A65098A37A59C06`. **Project GUID:** `6A65098A37A59C06` (locally checked).
- **Installed version:** 0.9.94. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** member-selected.
- **Purpose:** Newer Minnesinger headwear pack selected during the review.
- **Direct external dependencies:** [Zeliks Character](https://reforger.armaplatform.com/workshop/5D0551624969C92E) (`5D0551624969C92E`); [Minnesinger Core](https://reforger.armaplatform.com/workshop/6A6507D034ECFC69) (`6A6507D034ECFC69`); [NV-System](https://reforger.armaplatform.com/workshop/64722DADC53CB75E) (`64722DADC53CB75E`)
- **Additional transitive dependencies:** [Attachment Framework](https://reforger.armaplatform.com/workshop/645F08FA9E7CDEDE) (`645F08FA9E7CDEDE`); [Attachment Framework-Core](https://reforger.armaplatform.com/workshop/687CD82F6E41D627) (`687CD82F6E41D627`)
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Retain for controlled testing as the member preference; production approval remains with Weston.
- **Problems / compatibility:** Selected newer family; requires newer Core and Zeliks.

#### [MNSGR Ghillies — STANDALONE](https://reforger.armaplatform.com/workshop/6A3B3A10920F7DBA)

- **Workshop ID:** `6A3B3A10920F7DBA`. **Project GUID:** `6A3B3A10920F7DBA` (locally checked).
- **Installed version:** 1.0.0. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** retained candidate.
- **Purpose:** Standalone Minnesinger ghillie clothing.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Inspect and test before deciding on production inclusion.
- **Problems / compatibility:** Retained pending comparison with selected Clothes; possible content duplication, not a proven runtime conflict.

#### [GRS_BEARDS (formerly listed as CHAMPS_BEARDS)](https://reforger.armaplatform.com/workshop/68AB562502EB08A3)

- **Workshop ID:** `68AB562502EB08A3`. **Project GUID:** `68AB562502EB08A3` (locally checked).
- **Installed version:** 1.0.7. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** retained candidate.
- **Purpose:** Beard/character appearance content using Zeliks Character; previously recorded as CHAMPS_BEARDS.
- **Direct external dependencies:** [Zeliks Character](https://reforger.armaplatform.com/workshop/5D0551624969C92E) (`5D0551624969C92E`)
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Inspect and test before deciding on production inclusion.
- **Problems / compatibility:** Retained; character appearance.

### 3. Patches and carryable flags

#### [Flag Patches — Country](https://reforger.armaplatform.com/workshop/78C24C775ECE66C3)

- **Workshop ID:** `78C24C775ECE66C3`. **Project GUID:** `78C24C775ECE66C3` (locally checked).
- **Installed version:** 2.0.0. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** retained candidate.
- **Purpose:** Country flag patches built on GRS Patches and its framework.
- **Direct external dependencies:** [GRS — Patches](https://reforger.armaplatform.com/workshop/657B064AE0E231DF) (`657B064AE0E231DF`); [GRS — Dev Framework](https://reforger.armaplatform.com/workshop/65DACC64CE785B6C) (`65DACC64CE785B6C`)
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Inspect and test before deciding on production inclusion.
- **Problems / compatibility:** Retained exact supplied ID; requires [GRS Patches](https://reforger.armaplatform.com/workshop/657B064AE0E231DF) and [GRS Dev Framework](https://reforger.armaplatform.com/workshop/65DACC64CE785B6C).

#### [GRS — Patches](https://reforger.armaplatform.com/workshop/657B064AE0E231DF)

- **Workshop ID:** `657B064AE0E231DF`. **Project GUID:** `657B064AE0E231DF` (locally checked).
- **Installed version:** 1.0.26. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** required dependency.
- **Purpose:** GRS patch package in the country-flag patch dependency chain; exact complete patch inventory is not established here.
- **Direct external dependencies:** [GRS — Dev Framework](https://reforger.armaplatform.com/workshop/65DACC64CE785B6C) (`65DACC64CE785B6C`)
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Retain only if its parent content is approved; no production registration recommended by this research.
- **Problems / compatibility:** The web reader returned 404 on 2026-10-02. Preserve the previously checked ID and check in-game availability; do not silently omit a required dependency.

#### [SlavicWar Caryable Flags](https://reforger.armaplatform.com/workshop/6A6B2DB5E4EFDCDF)

- **Workshop ID:** `6A6B2DB5E4EFDCDF`. **Project GUID:** `6A6B2DB5E4EFDCDF` (locally checked).
- **Installed version:** 1.0.0. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** retained candidate.
- **Purpose:** SlavicWar carryable flag content built on Caryable Flags.
- **Direct external dependencies:** [Caryable Flags](https://reforger.armaplatform.com/workshop/62BF3C345D95EF63) (`62BF3C345D95EF63`)
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Inspect and test before deciding on production inclusion.
- **Problems / compatibility:** Retained; requires [Caryable Flags](https://reforger.armaplatform.com/workshop/62BF3C345D95EF63).

#### [Caryable Flags](https://reforger.armaplatform.com/workshop/62BF3C345D95EF63)

- **Workshop ID:** `62BF3C345D95EF63`. **Project GUID:** `62BF3C345D95EF63` (locally checked).
- **Installed version:** 1.0.0. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** required dependency.
- **Purpose:** Base carryable flag mod.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Retain only if its parent content is approved; no production registration recommended by this research.
- **Problems / compatibility:** No individual failure established by this research. Full-stack behavior remains untested.

### 4. Firearms and functional weapon packages

#### [Chungus Shotguns](https://reforger.armaplatform.com/workshop/620E584B1D2C96A4)

- **Workshop ID:** `620E584B1D2C96A4`. **Project GUID:** `620E584B1D2C96A4` (locally checked).
- **Installed version:** 1.0.43. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** retained candidate.
- **Purpose:** Pump-action and double-barrel shotgun content.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Inspect and test before deciding on production inclusion.
- **Problems / compatibility:** Retained; shotgun content.

#### [RGM-40_GL-06](https://reforger.armaplatform.com/workshop/69148A1D52696666)

- **Workshop ID:** `69148A1D52696666`. **Project GUID:** `69148A1D52696666` (locally checked).
- **Installed version:** 1.0.9. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** retained candidate.
- **Purpose:** RGM-40 and GL-06 grenade launchers.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Inspect and test before deciding on production inclusion.
- **Problems / compatibility:** Retained; grenade launchers.

#### [GRS Weapons 2](https://reforger.armaplatform.com/workshop/E4D206B81482F59E)

- **Workshop ID:** `E4D206B81482F59E`. **Project GUID:** `E4D206B81482F59E` (locally checked).
- **Installed version:** 1.0.20. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** retained candidate.
- **Purpose:** GRS rifle/weapon content pack.
- **Direct external dependencies:** [GRS Suppressor Pack](https://reforger.armaplatform.com/workshop/6A30C1606411936A) (`6A30C1606411936A`)
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Inspect and test before deciding on production inclusion.
- **Problems / compatibility:** Retained; rifle pack.

#### [HK-G95KA1](https://reforger.armaplatform.com/workshop/6A3331DC109F8B6F)

- **Workshop ID:** `6A3331DC109F8B6F`. **Project GUID:** `6A3331DC109F8B6F` (locally checked).
- **Installed version:** 1.0.0. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** retained candidate.
- **Purpose:** HK G95KA1 rifle content.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Inspect and test before deciding on production inclusion.
- **Problems / compatibility:** Retained; German rifle content.

#### [MSBS Grot](https://reforger.armaplatform.com/workshop/6590ECF80DD17257)

- **Workshop ID:** `6590ECF80DD17257`. **Project GUID:** `6590ECF80DD17257` (locally checked).
- **Installed version:** 0.0.40. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** retained candidate.
- **Purpose:** MSBS Grot rifle content.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Inspect and test before deciding on production inclusion.
- **Problems / compatibility:** Retained; rifle content.

#### [MCR-Pack 5.56x45](https://reforger.armaplatform.com/workshop/5D7531C2D05BC700)

- **Workshop ID:** `5D7531C2D05BC700`. **Project GUID:** `5D7531C2D05BC700` (locally checked).
- **Installed version:** 1.0.11. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** member-selected.
- **Purpose:** 5.56×45 MCR weapon pack with DMR, SAW, CQC and Carbine variants, multiple finishes, RIS attachment/optic compatibility and suppressor support described by the author.
- **Direct external dependencies:** [Bacon Suppressors](https://reforger.armaplatform.com/workshop/5AB301290317994A) (`5AB301290317994A`); [RIS Laser Attachments](https://reforger.armaplatform.com/workshop/5ABD0CB57F7E9EB1) (`5ABD0CB57F7E9EB1`)
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Retain for controlled testing as the member preference; production approval remains with Weston.
- **Problems / compatibility:** Selected by user on 2026-10-02; DMR, SAW, CQC and Carbine variants, multiple finishes, RIS optic/attachment and suppressor support described by author. Requires Bacon Suppressors (already retained) and RIS Laser Attachments (new dependency). Published v1.0.11 targets game 1.2.0.70 and was last modified 2024-06-26; current Tools/server compatibility is untested. Added by your request on 2026-10-02. Published v1.0.11 targets Reforger 1.2.0.70 and was last modified 2024-06-26, so present-day Tools/server compatibility needs a local test. Published dependency chain was read on 2026-10-02; no additional prerequisites are listed on either dependency page. This does not certify universal RIS, sound, recoil or heating compatibility.

#### [FN SCAR — Heavy and Light](https://reforger.armaplatform.com/workshop/616478A18DC7DCB6)

- **Workshop ID:** `616478A18DC7DCB6`. **Project GUID:** `616478A18DC7DCB6` (locally checked).
- **Installed version:** 1.6.7. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** member-selected.
- **Purpose:** SCAR-H 7.62mm NATO and SCAR-L 5.56×45 pack with FDE/Black/CQC variants, suppressors, foregrips, custom SCAR-H magazines and animations.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Retain for controlled testing as the member preference; production approval remains with Weston.
- **Problems / compatibility:** Selected by user on 2026-10-02; SCAR-H 7.62mm NATO and SCAR-L 5.56×45, FDE/Black/CQC variants, custom suppressors, foregrips, SCAR-H magazines and animations. Author describes RIS attachment support. No external Workshop dependencies listed. Published v1.6.7 targets game 1.3.0.170, last modified 2025-05-10; current Tools/server compatibility is untested. Added by your request on 2026-10-02. The author describes RIS optics/light/laser support and flippable iron sights. Published v1.6.7 targets Reforger 1.3.0.170, last modified 2025-05-10. No external dependencies are listed on the page as read on 2026-10-02; current attachment, animation, sound, recoil and heating compatibility remains untested.

#### [FORTEX RTW](https://reforger.armaplatform.com/workshop/68F92DCEEC950E1C)

- **Workshop ID:** `68F92DCEEC950E1C`. **Project GUID:** `68F92DCEEC950E1C` (locally checked).
- **Installed version:** 1.0.28. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** required dependency.
- **Purpose:** Russian Tactical Weapon asset pack. The author directs users to RU Weapon Core for the functional weapon package.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Retain only if its parent content is approved; no production registration recommended by this research.
- **Problems / compatibility:** No individual failure established by this research. Full-stack behavior remains untested.

#### [FORTEX — RU Weapon Core](https://reforger.armaplatform.com/workshop/690AC3895BAAFAE1)

- **Workshop ID:** `690AC3895BAAFAE1`. **Project GUID:** `690AC3895BAAFAE1` (locally checked).
- **Installed version:** 1.0.91. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** required dependency.
- **Purpose:** Functional FORTEX Russian weapon package: AK-family rifles and SVD-M, custom attachments/sounds, subsonic ammunition and other systems.
- **Direct external dependencies:** [FORTEX RTW](https://reforger.armaplatform.com/workshop/68F92DCEEC950E1C) (`68F92DCEEC950E1C`)
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Retain only if its parent content is approved; no production registration recommended by this research.
- **Problems / compatibility:** Newly recorded feature overlap: the author lists suppressor heating/degradation, while 2-7 is also retained. A user choice is pending; both remain candidates and combined behavior is untested.

### 5. Magazines, suppressors and weapon visuals

#### [Extended Fortex Taped Mags](https://reforger.armaplatform.com/workshop/6A3B4D970AC5EC13)

- **Workshop ID:** `6A3B4D970AC5EC13`. **Project GUID:** `6A3B4D970AC5EC13` (locally checked).
- **Installed version:** 1.0.1. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** retained candidate.
- **Purpose:** Extended taped magazines for the FORTEX weapon ecosystem.
- **Direct external dependencies:** [FORTEX — RU Weapon Core](https://reforger.armaplatform.com/workshop/690AC3895BAAFAE1) (`690AC3895BAAFAE1`); [FORTEX Framework](https://reforger.armaplatform.com/workshop/66C2ED22F25B05C9) (`66C2ED22F25B05C9`); [FORTEX RTW](https://reforger.armaplatform.com/workshop/68F92DCEEC950E1C) (`68F92DCEEC950E1C`)
- **Additional transitive dependencies:** [NV-System](https://reforger.armaplatform.com/workshop/64722DADC53CB75E) (`64722DADC53CB75E`); [Attachment Framework](https://reforger.armaplatform.com/workshop/645F08FA9E7CDEDE) (`645F08FA9E7CDEDE`); [Attachment Framework-Core](https://reforger.armaplatform.com/workshop/687CD82F6E41D627) (`687CD82F6E41D627`)
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Inspect and test before deciding on production inclusion.
- **Problems / compatibility:** Retained; requires FORTEX Framework, RTW and RU Weapon Core. The 2-7/FORTEX heating overlap comes through this package’s RU Weapon Core dependency. Removing a required dependency while keeping this mod would leave an incomplete setup.

#### [AK74 Retextures](https://reforger.armaplatform.com/workshop/6A5D18F82B3210CD)

- **Workshop ID:** `6A5D18F82B3210CD`. **Project GUID:** `6A5D18F82B3210CD` (locally checked).
- **Installed version:** 1.0.10. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** retained candidate.
- **Purpose:** AK74 texture/retexture content.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Inspect and test before deciding on production inclusion.
- **Problems / compatibility:** Retained; texture content.

#### [Bacon Suppressors](https://reforger.armaplatform.com/workshop/5AB301290317994A)

- **Workshop ID:** `5AB301290317994A`. **Project GUID:** `5AB301290317994A` (locally checked).
- **Installed version:** 1.2.7. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** retained candidate.
- **Purpose:** Suppressor attachment content for compatible weapons.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Inspect and test before deciding on production inclusion.
- **Problems / compatibility:** Retained; suppressor content for compatible weapons, distinct from heat mechanics.

#### [GRS Suppressor Pack](https://reforger.armaplatform.com/workshop/6A30C1606411936A)

- **Workshop ID:** `6A30C1606411936A`. **Project GUID:** `6A30C1606411936A` (locally checked).
- **Installed version:** 1.0.2. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** required dependency.
- **Purpose:** GRS suppressor attachment pack. The page is marked WIP and does not provide a detailed item inventory.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Retain only if its parent content is approved; no production registration recommended by this research.
- **Problems / compatibility:** No individual failure established by this research. Full-stack behavior remains untested.

#### [RIS Laser Attachments](https://reforger.armaplatform.com/workshop/5ABD0CB57F7E9EB1)

- **Workshop ID:** `5ABD0CB57F7E9EB1`. **Project GUID:** `5ABD0CB57F7E9EB1` (locally checked).
- **Installed version:** 1.8.0. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** required dependency.
- **Purpose:** Weapon-mounted laser/flashlight framework and attachments with a radial menu and some optics; requires compatible weapons in use.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Retain only if its parent content is approved; no production registration recommended by this research.
- **Problems / compatibility:** Published v1.8.0 targets Reforger 1.8.0.10. No external dependencies are listed on its Workshop page as read on 2026-10-02. Compatibility with the selected Attachment Framework ecosystem is untested, not a proven conflict.

### 6. Thermal optics, ranging and their faction patch

#### [SW-Reap-IR thermal scope](https://reforger.armaplatform.com/workshop/695A2C3D78F0B612)

- **Workshop ID:** `695A2C3D78F0B612`. **Project GUID:** `695A2C3D78F0B612` (locally checked).
- **Installed version:** 1.0.24. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** retained candidate.
- **Purpose:** SW-Reap-IR thermal sight candidate.
- **Direct external dependencies:** [RHS — Status Quo](https://reforger.armaplatform.com/workshop/595F2BF2F44836FB) (`595F2BF2F44836FB`); [Laser Rangefinder](https://reforger.armaplatform.com/workshop/620E426C34BE0D17) (`620E426C34BE0D17`); [Ukraine Armed Forces RHS Fix](https://reforger.armaplatform.com/workshop/673C00E41ACDC2D4) (`673C00E41ACDC2D4`)
- **Additional transitive dependencies:** [RHS — Content Pack 01](https://reforger.armaplatform.com/workshop/1337C0DE5DABBEEF) (`1337C0DE5DABBEEF`); [RHS — Content Pack 02](https://reforger.armaplatform.com/workshop/BADC0DEDABBEDA5E) (`BADC0DEDABBEDA5E`); [Ukraine Armed Forces RHS](https://reforger.armaplatform.com/workshop/65F92D51845AC237) (`65F92D51845AC237`)
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Inspect and test before deciding on production inclusion.
- **Problems / compatibility:** No individual failure established by this research. Full-stack behavior remains untested.

#### [VooDoo-S Thermal Clip-On](https://reforger.armaplatform.com/workshop/CEF13D07B83D0A23)

- **Workshop ID:** `CEF13D07B83D0A23`. **Project GUID:** `CEF13D07B83D0A23` (locally checked).
- **Installed version:** 1.0.1. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** retained candidate.
- **Purpose:** Thermal clip-on with white-hot imaging, flip-to-side operation and PiP/2D modes.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Inspect and test before deciding on production inclusion.
- **Problems / compatibility:** Retained; compatible modded RIS rifle + RIS scope of at least3x required. NV/thermal scopes unsupported. White-hot, flip-to-side, PiP/2D. Author uses local clear-weather rendering while aiming to avoid flicker.

#### [Laser Rangefinder](https://reforger.armaplatform.com/workshop/620E426C34BE0D17)

- **Workshop ID:** `620E426C34BE0D17`. **Project GUID:** `620E426C34BE0D17` (locally checked).
- **Installed version:** 1.0.12. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** required dependency.
- **Purpose:** Laser-rangefinder scope component for other mods and a standalone rangefinder unit.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Retain only if its parent content is approved; no production registration recommended by this research.
- **Problems / compatibility:** No individual failure established by this research. Full-stack behavior remains untested.

#### [Ukraine Armed Forces RHS Fix](https://reforger.armaplatform.com/workshop/673C00E41ACDC2D4)

- **Workshop ID:** `673C00E41ACDC2D4`. **Project GUID:** `673C00E41ACDC2D4` (locally checked).
- **Installed version:** 1.0.16. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** required dependency.
- **Purpose:** Faction replacement package: its author says it replaces US with Ukrainian and USSR with Russian factions.
- **Direct external dependencies:** [Ukraine Armed Forces RHS](https://reforger.armaplatform.com/workshop/65F92D51845AC237) (`65F92D51845AC237`)
- **Additional transitive dependencies:** [RHS — Status Quo](https://reforger.armaplatform.com/workshop/595F2BF2F44836FB) (`595F2BF2F44836FB`); [RHS — Content Pack 01](https://reforger.armaplatform.com/workshop/1337C0DE5DABBEEF) (`1337C0DE5DABBEEF`); [RHS — Content Pack 02](https://reforger.armaplatform.com/workshop/BADC0DEDABBEDA5E) (`BADC0DEDABBEDA5E`)
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Retain only if its parent content is approved; no production registration recommended by this research.
- **Problems / compatibility:** This does more than fix an optic: the published description says it changes the US/USSR factions. Your fictional team names and custom faction configuration remain undecided; test replacement behavior before integration.

### 7. Grenades, smoke and mines

#### [Extra Grenades](https://reforger.armaplatform.com/workshop/66B500DF95F2C3AF)

- **Workshop ID:** `66B500DF95F2C3AF`. **Project GUID:** `66B500DF95F2C3AF` (locally checked).
- **Installed version:** 1.2.7. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** member-selected.
- **Purpose:** Combined grenade content covering the listed Eastern/Improvised packs plus DM51, GHO-1 and M26.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Retain for controlled testing as the member preference; production approval remains with Weston.
- **Problems / compatibility:** Selected; includes the listed Eastern/Improvised content, plus DM51, GHO-1 and M26.

#### [Extra Impact Grenades](https://reforger.armaplatform.com/workshop/68A0F788C81285BF)

- **Workshop ID:** `68A0F788C81285BF`. **Project GUID:** `68A0F788C81285BF` (locally checked).
- **Installed version:** 1.0.6. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** member-selected.
- **Purpose:** Additional impact grenades: RGO, RGN and RKG-3.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Retain for controlled testing as the member preference; production approval remains with Weston.
- **Problems / compatibility:** Selected separate pack; RGO, RGN, RKG-3.

#### [Extra Mines](https://reforger.armaplatform.com/workshop/66C75538DD0251D1)

- **Workshop ID:** `66C75538DD0251D1`. **Project GUID:** `66C75538DD0251D1` (locally checked).
- **Installed version:** 1.1.0. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** member-selected.
- **Purpose:** Mine content including PMN-2, PFM-1 and OZM-72.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Retain for controlled testing as the member preference; production approval remains with Weston.
- **Problems / compatibility:** Selected separate pack; PMN-2, PFM-1, OZM-72.

#### [Mark 1 Thermobaric Grenade](https://reforger.armaplatform.com/workshop/68E53DBBFC8EFC78)

- **Workshop ID:** `68E53DBBFC8EFC78`. **Project GUID:** `68E53DBBFC8EFC78` (locally checked).
- **Installed version:** 1.0.6. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** retained candidate.
- **Purpose:** Mark 1 thermobaric grenade content.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Inspect and test before deciding on production inclusion.
- **Problems / compatibility:** Retained; additional grenade content.

#### [RDG-2 Colored Smoke Grenades](https://reforger.armaplatform.com/workshop/6A5E08CF942970BB)

- **Workshop ID:** `6A5E08CF942970BB`. **Project GUID:** `6A5E08CF942970BB` (locally checked).
- **Installed version:** 1.0.3. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** retained candidate.
- **Purpose:** Four colored RDG-2 smoke-grenade variants.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Inspect and test before deciding on production inclusion.
- **Problems / compatibility:** Retained; four colored variants.

### 8. Vehicles and drones

#### [Ukrainian War Vehicles](https://reforger.armaplatform.com/workshop/699E29EBC9B02967)

- **Workshop ID:** `699E29EBC9B02967`. **Project GUID:** `699E29EBC9B02967` (locally checked).
- **Installed version:** 0.0.7. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** retained candidate.
- **Purpose:** Ukrainian War Vehicles content from Red Thunder System.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Inspect and test before deciding on production inclusion.
- **Problems / compatibility:** Retained; Red Thunder System vehicle content.

#### [RUVicExpansion](https://reforger.armaplatform.com/workshop/6A295E0AF0B307E7)

- **Workshop ID:** `6A295E0AF0B307E7`. **Project GUID:** `6A295E0AF0B307E7` (locally checked).
- **Installed version:** 1.0.8. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** retained candidate.
- **Purpose:** Armed/armored UAZ-452, three UAZ-469 variants and modified BRDM-2 content.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Inspect and test before deciding on production inclusion.
- **Problems / compatibility:** Retained; armed/armored UAZ-452, three UAZ-469 variants and modified BRDM-2.

#### [VT4 — FRM](https://reforger.armaplatform.com/workshop/663B2784961621FB)

- **Workshop ID:** `663B2784961621FB`. **Project GUID:** `663B2784961621FB` (locally checked).
- **Installed version:** 1.0.11. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** retained candidate.
- **Purpose:** VT4 light transport/logistics vehicles, including BTDF and CE variants with regular and Equipped versions.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** Member reports a successful BTDF drive test in FritV3; placement/local child confirmed. Team assignment, server use and error-free behavior unverified.
- **Recommendation:** Inspect and test before deciding on production inclusion.
- **Problems / compatibility:** Retained; WIP light transport/logistics. You reported a successful drive test in FritV3. A local child of BTDF exists. The two-team assignment and server deployment are not yet verified. “Equipped” contents have not been inspected; do not assume it means armed.

#### [M1224 Maxxpro MRAP 1.8 Fix](https://reforger.armaplatform.com/workshop/6A5169730316933C)

- **Workshop ID:** `6A5169730316933C`. **Project GUID:** `6A5169730316933C` (locally checked).
- **Installed version:** 1.0.2. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** retained candidate.
- **Purpose:** Reforger 1.8 fix addon for the M1224 Maxxpro MRAP base mod.
- **Direct external dependencies:** [M1224 Maxxpro MRAP](https://reforger.armaplatform.com/workshop/684D34D51DC5E22A) (`684D34D51DC5E22A`); [PR_UTILS](https://reforger.armaplatform.com/workshop/686104581D2D722B) (`686104581D2D722B`)
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Inspect and test before deciding on production inclusion.
- **Problems / compatibility:** Needs testing; supplied ID is a fix requiring [base MRAP](https://reforger.armaplatform.com/workshop/684D34D51DC5E22A) and [PR_UTILS](https://reforger.armaplatform.com/workshop/686104581D2D722B). Author says M134 turret remains unfixed because PR Utils is outdated.

#### [M1224 Maxxpro MRAP](https://reforger.armaplatform.com/workshop/684D34D51DC5E22A)

- **Workshop ID:** `684D34D51DC5E22A`. **Project GUID:** `684D34D51DC5E22A` (locally checked).
- **Installed version:** 0.0.8. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** required dependency.
- **Purpose:** Base M1224 Maxxpro MRAP vehicle package.
- **Direct external dependencies:** [PR_UTILS](https://reforger.armaplatform.com/workshop/686104581D2D722B) (`686104581D2D722B`)
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Retain only if its parent content is approved; no production registration recommended by this research.
- **Problems / compatibility:** No individual failure established by this research. Full-stack behavior remains untested.

#### [PR_UTILS](https://reforger.armaplatform.com/workshop/686104581D2D722B)

- **Workshop ID:** `686104581D2D722B`. **Project GUID:** `686104581D2D722B` (locally checked).
- **Installed version:** 0.0.3. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** required dependency.
- **Purpose:** Project Realism vehicle support scripts, models and configurations.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Retain only if its parent content is approved; no production registration recommended by this research.
- **Problems / compatibility:** No individual failure established by this research. Full-stack behavior remains untested.

#### [Realistic Combat Drones](https://reforger.armaplatform.com/workshop/65AD60E204191D37)

- **Workshop ID:** `65AD60E204191D37`. **Project GUID:** `65AD60E204191D37` (locally checked).
- **Installed version:** 2.3.7. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** retained candidate.
- **Purpose:** Recon/FPV combat drones with batteries and signal/jamming systems.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Inspect and test before deciding on production inclusion.
- **Problems / compatibility:** Retained original package matching the supplied name, SalamiBoiDev. Recon/FPV drones, signal/jamming and batteries. v2.3.7/game1.7.0.54, updated2026-08-02. Original package used as the draft assumption; combined runtime untested.

### 9. Medical — your selected ACE Dev set

#### [ACE Core Dev](https://reforger.armaplatform.com/workshop/65AD7D0D9941A380)

- **Workshop ID:** `65AD7D0D9941A380`. **Project GUID:** `65AD7D0D9941A380` (locally checked).
- **Installed version:** 1.5.39. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** member-selected.
- **Purpose:** Shared ACE Dev foundation used by the selected medical components.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Retain for controlled testing as the member preference; production approval remains with Weston.
- **Problems / compatibility:** Selected required foundation.

#### [ACE Medical Core Dev](https://reforger.armaplatform.com/workshop/6586079789278413)

- **Workshop ID:** `6586079789278413`. **Project GUID:** `6586079789278413` (locally checked).
- **Installed version:** 1.5.36. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** member-selected.
- **Purpose:** Medical backbone with unconsciousness/Second Chance, pain and medication behavior, patient repositioning and medical interactions.
- **Direct external dependencies:** [ACE Core Dev](https://reforger.armaplatform.com/workshop/65AD7D0D9941A380) (`65AD7D0D9941A380`)
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Retain for controlled testing as the member preference; production approval remains with Weston.
- **Problems / compatibility:** Selected medical backbone.

#### [ACE Medical Hitzones Dev](https://reforger.armaplatform.com/workshop/65B343F799FB521B)

- **Workshop ID:** `65B343F799FB521B`. **Project GUID:** `65B343F799FB521B` (locally checked).
- **Installed version:** 1.5.36. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** member-selected.
- **Purpose:** Medical hitzone overhaul with organ/artery hitzones and changed fatal-injury evaluation.
- **Direct external dependencies:** [ACE Medical Core Dev](https://reforger.armaplatform.com/workshop/6586079789278413) (`6586079789278413`)
- **Additional transitive dependencies:** [ACE Core Dev](https://reforger.armaplatform.com/workshop/65AD7D0D9941A380) (`65AD7D0D9941A380`)
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Retain for controlled testing as the member preference; production approval remains with Weston.
- **Problems / compatibility:** Selected injury/hitzone component.

#### [ACE Medical Circulation Dev](https://reforger.armaplatform.com/workshop/65AD7D4F994EA327)

- **Workshop ID:** `65AD7D4F994EA327`. **Project GUID:** `65AD7D4F994EA327` (locally checked).
- **Installed version:** 1.5.36. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** member-selected.
- **Purpose:** Heart-rate and blood-pressure simulation, cardiac arrest, CPR and more advanced medication effects.
- **Direct external dependencies:** [ACE Medical Core Dev](https://reforger.armaplatform.com/workshop/6586079789278413) (`6586079789278413`)
- **Additional transitive dependencies:** [ACE Core Dev](https://reforger.armaplatform.com/workshop/65AD7D0D9941A380) (`65AD7D0D9941A380`)
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Retain for controlled testing as the member preference; production approval remains with Weston.
- **Problems / compatibility:** Selected advanced circulation/drugs/CPR.

#### [ACE Medical Breathing Dev](https://reforger.armaplatform.com/workshop/671F73D99978B4F2)

- **Workshop ID:** `671F73D99978B4F2`. **Project GUID:** `671F73D99978B4F2` (locally checked).
- **Installed version:** 1.5.36. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** member-selected.
- **Purpose:** Respiratory simulation, airway obstruction and chest injuries; adds airway/chest treatment equipment.
- **Direct external dependencies:** [ACE Medical Circulation Dev](https://reforger.armaplatform.com/workshop/65AD7D4F994EA327) (`65AD7D4F994EA327`)
- **Additional transitive dependencies:** [ACE Medical Core Dev](https://reforger.armaplatform.com/workshop/6586079789278413) (`6586079789278413`); [ACE Core Dev](https://reforger.armaplatform.com/workshop/65AD7D0D9941A380) (`65AD7D0D9941A380`)
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Retain for controlled testing as the member preference; production approval remains with Weston.
- **Problems / compatibility:** Selected breathing component.

#### [SplintACE](https://reforger.armaplatform.com/workshop/697CC096F5E423E0)

- **Workshop ID:** `697CC096F5E423E0`. **Project GUID:** `697CC096F5E423E0` (locally checked).
- **Installed version:** 1.0.7. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** member-selected.
- **Purpose:** Third-party ACE-compatible splint addon with regeneration behavior.
- **Direct external dependencies:** [ACE Core Dev](https://reforger.armaplatform.com/workshop/65AD7D0D9941A380) (`65AD7D0D9941A380`); [ACE Medical Hitzones Dev](https://reforger.armaplatform.com/workshop/65B343F799FB521B) (`65B343F799FB521B`); [ACE Medical Circulation Dev](https://reforger.armaplatform.com/workshop/65AD7D4F994EA327) (`65AD7D4F994EA327`); [ACE Medical Core Dev](https://reforger.armaplatform.com/workshop/6586079789278413) (`6586079789278413`)
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Retain for controlled testing as the member preference; production approval remains with Weston.
- **Problems / compatibility:** Selected third-party splint addon; requires the matching Dev prerequisites above.

### 10. Movement, camera, recoil and unconsciousness

#### [CERTII-SimulatedMovement](https://reforger.armaplatform.com/workshop/69B252D91D8172C2)

- **Workshop ID:** `69B252D91D8172C2`. **Project GUID:** `69B252D91D8172C2` (locally checked).
- **Installed version:** 7.0.10. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** member-selected.
- **Purpose:** Camera/inertia/fatigue, injury reactions and weapon drops, blast knockdowns, and weight/stamina changes.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** Member reports CERTII + Best Body Cam work together; scenario/settings were not recorded. Codex has not independently reproduced it; full-stack compatibility remains unknown.
- **Recommendation:** Retain for controlled testing as the member preference; production approval remains with Weston.
- **Problems / compatibility:** Selected with CERTII HUD and Grenade Limiter. Replaces the earlier Bodycam-first comparison plan. Camera/inertia/fatigue, injury reactions/weapon drops, blast knockdowns and weight/stamina changes; test with ACE, GC and ragdolls. No published Workshop dependencies.

#### [Best Body Cam](https://reforger.armaplatform.com/workshop/64FD308B60F7B687)

- **Workshop ID:** `64FD308B60F7B687`. **Project GUID:** `64FD308B60F7B687` (locally checked).
- **Installed version:** 1.0.9. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** member-selected.
- **Purpose:** Bodycam-view candidate; the author lists Ctrl+C, but gives no detailed description of its camera behavior.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** Member reports CERTII + Best Body Cam work together; scenario/settings were not recorded. Codex has not independently reproduced it; full-stack compatibility remains unknown.
- **Recommendation:** Retain for controlled testing as the member preference; production approval remains with Weston.
- **Problems / compatibility:** Selected by user on 2026-10-02 for combined testing with CERTII-SimulatedMovement; separate from the excluded SlavicWar Bodycam. Author lists Ctrl+C; detailed effect behavior is not described. No external Workshop dependencies listed. Published v1.0.9 targets game 1.3.0.130, last modified 2025-04-06. Potential camera overlap remains a test item, not a proven runtime conflict.

#### [improved reforger ragdolls](https://reforger.armaplatform.com/workshop/64594B4F6C4E9718)

- **Workshop ID:** `64594B4F6C4E9718`. **Project GUID:** `64594B4F6C4E9718` (locally checked).
- **Installed version:** 1.0.7. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** retained candidate.
- **Purpose:** Character ragdoll joint/physics changes.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Inspect and test before deciding on production inclusion.
- **Problems / compatibility:** Retained; joint/physics changes, combined injury/ragdoll tests needed.

#### [TGZ_RecoilRealism](https://reforger.armaplatform.com/workshop/699923D8F24A15D0)

- **Workshop ID:** `699923D8F24A15D0`. **Project GUID:** `699923D8F24A15D0` (locally checked).
- **Installed version:** 1.0.0. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** retained candidate.
- **Purpose:** Recoil-realism candidate with a sparse published description; exact weapon coverage and tuning are not established.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Inspect and test before deciding on production inclusion.
- **Problems / compatibility:** Retained; BK., v1.0.0/game1.7.0.49. Sparse description; weapon coverage and combined recoil/inertia behavior unverified.

#### [Keep Gun When Uncon](https://reforger.armaplatform.com/workshop/6088A3044B7ECBFD)

- **Workshop ID:** `6088A3044B7ECBFD`. **Project GUID:** `6088A3044B7ECBFD` (locally checked).
- **Installed version:** 1.0.1. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** retained candidate.
- **Purpose:** Attempts to keep/holster a weapon when the character becomes unconscious.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Inspect and test before deciding on production inclusion.
- **Problems / compatibility:** Retained pending injury tests; attempts unconscious holstering. Check interaction with ACE and CERTII weapon drops.

### 11. Suppression, suppressor heat and blood

#### [GC Suppression](https://reforger.armaplatform.com/workshop/684CE8AA3B1D6573)

- **Workshop ID:** `684CE8AA3B1D6573`. **Project GUID:** `684CE8AA3B1D6573` (locally checked).
- **Installed version:** 1.7.2. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** member-selected.
- **Purpose:** Configurable incoming-fire and explosion suppression effects, cover/vehicle handling and optional aiming flinch.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Retain for controlled testing as the member preference; production approval remains with Weston.
- **Problems / compatibility:** Selected; resolves original “GCS Suppression” name. Configurable incoming-fire/blast effects.

#### [2-7 Suppressor Overheating](https://reforger.armaplatform.com/workshop/66B073D763F66862)

- **Workshop ID:** `66B073D763F66862`. **Project GUID:** `66B073D763F66862` (locally checked).
- **Installed version:** 1.1.7. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** member-selected.
- **Purpose:** Suppressor heat, durability, damage/destruction and smoke/glow effects under sustained fire.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Retain for controlled testing as the member preference; production approval remains with Weston.
- **Problems / compatibility:** Selected; duplicate pasted entry collapsed. Heat/wear/smoke; server smoke effects do not work in offline GM/Workbench according to author.

#### [Improved Blood Effect Deluxe](https://reforger.armaplatform.com/workshop/660896EB172D4B7F)

- **Workshop ID:** `660896EB172D4B7F`. **Project GUID:** `660896EB172D4B7F` (locally checked).
- **Installed version:** 1.0.3. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** member-selected.
- **Purpose:** Deluxe blood-effect extension for Improved Blood Effect.
- **Direct external dependencies:** [Improved Blood Effect](https://reforger.armaplatform.com/workshop/62FCEB51DF8527B6) (`62FCEB51DF8527B6`)
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Retain for controlled testing as the member preference; production approval remains with Weston.
- **Problems / compatibility:** Selected for PC-focused testing. General multiplayer performance unmeasured.

#### [Improved Blood Effect](https://reforger.armaplatform.com/workshop/62FCEB51DF8527B6)

- **Workshop ID:** `62FCEB51DF8527B6`. **Project GUID:** `62FCEB51DF8527B6` (locally checked).
- **Installed version:** 1.5.9. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** member-selected.
- **Purpose:** Base Improved Blood Effect package.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Retain for controlled testing as the member preference; production approval remains with Weston.
- **Problems / compatibility:** Selected required base for Deluxe.

### 12. Combat sound, voices and earplugs

#### [Ukraine Radio Chatter](https://reforger.armaplatform.com/workshop/6532B17102092B01)

- **Workshop ID:** `6532B17102092B01`. **Project GUID:** `6532B17102092B01` (locally checked).
- **Installed version:** 0.0.1. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** retained candidate.
- **Purpose:** Ukrainian radio-chatter replacement.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Inspect and test before deciding on production inclusion.
- **Problems / compatibility:** Retained; radio replacement. Older published game target; current runtime check needed.

#### [Realistic Combat Sound Mod](https://reforger.armaplatform.com/workshop/68C6D7DD75DBDB57)

- **Workshop ID:** `68C6D7DD75DBDB57`. **Project GUID:** `68C6D7DD75DBDB57` (locally checked).
- **Installed version:** 0.5.8. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** member-selected.
- **Purpose:** Combat sound overhaul; author describes support for RHS/ARMA-RY content.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Retain for controlled testing as the member preference; production approval remains with Weston.
- **Problems / compatibility:** Selected main sound overhaul; author claims RHS/ARMA-RY support.

#### [Screams of War](https://reforger.armaplatform.com/workshop/69E67B11DA454B9E)

- **Workshop ID:** `69E67B11DA454B9E`. **Project GUID:** `69E67B11DA454B9E` (locally checked).
- **Installed version:** 1.0.3. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** member-selected.
- **Purpose:** Injury and fatal voice effects; author lists ACE Medical Dev compatibility.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Retain for controlled testing as the member preference; production approval remains with Weston.
- **Problems / compatibility:** Selected; injury/fatal voices, claimed ACE Medical Dev compatibility.

#### [LG_EarPlugs](https://reforger.armaplatform.com/workshop/6A14E4F10EA40001)

- **Workshop ID:** `6A14E4F10EA40001`. **Project GUID:** `6A14E4F10EA40001` (locally checked).
- **Installed version:** 1.0.2. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** retained candidate.
- **Purpose:** Toggleable earplug sound reduction, listed with an F2 control.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Inspect and test before deciding on production inclusion.
- **Problems / compatibility:** Retained; F2 earplugs by armalucky. Exact name now verified.

### 13. Maps, ATAK and gear compatibility

#### [Player Map Markers](https://reforger.armaplatform.com/workshop/5E92F5A4A1B75A75)

- **Workshop ID:** `5E92F5A4A1B75A75`. **Project GUID:** `5E92F5A4A1B75A75` (locally checked).
- **Installed version:** 1.3.2. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** retained candidate.
- **Purpose:** Player and occupied-vehicle map markers.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Inspect and test before deciding on production inclusion.
- **Problems / compatibility:** Retained; players/occupied-vehicle markers. Map visibility remains an NML policy decision.

#### [Where Am I](https://reforger.armaplatform.com/workshop/5965550F24A0C152)

- **Workshop ID:** `5965550F24A0C152`. **Project GUID:** `5965550F24A0C152` (locally checked).
- **Installed version:** 1.2.0. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** retained candidate.
- **Purpose:** Own map position/direction and a map-centering button.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Inspect and test before deciding on production inclusion.
- **Problems / compatibility:** Retained; own map position/direction and center button. Older published target; runtime check needed.

#### [Kova x ATAK](https://reforger.armaplatform.com/workshop/6A49417788E13618)

- **Workshop ID:** `6A49417788E13618`. **Project GUID:** `6A49417788E13618` (locally checked).
- **Installed version:** 0.0.8. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** required dependency.
- **Purpose:** ATAK-style battlefield device with maps, symbols, waypoints/drawing, server-configurable team tracking, chat and a chest-mounted map.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Retain only if its parent content is approved; no production registration recommended by this research.
- **Problems / compatibility:** Required base identified through both ATAK patches; own full dependency inspection pending.

#### [ATAK x COMPAT](https://reforger.armaplatform.com/workshop/6A756C77B915750B)

- **Workshop ID:** `6A756C77B915750B`. **Project GUID:** `6A756C77B915750B` (locally checked).
- **Installed version:** 1.0.0. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** retained candidate.
- **Purpose:** Shared ATAK compatibility base for devices/vest slots; provides no supported vest collection by itself.
- **Direct external dependencies:** [Kova x ATAK](https://reforger.armaplatform.com/workshop/6A49417788E13618) (`6A49417788E13618`)
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Inspect and test before deciding on production inclusion.
- **Problems / compatibility:** Retained shared base for compatible devices/vest slots; requires Kova x ATAK. No supported vests by itself.

#### [Minnesinger X ATAK Compat](https://reforger.armaplatform.com/workshop/6A7567D790CA7DBF)

- **Workshop ID:** `6A7567D790CA7DBF`. **Project GUID:** `6A7567D790CA7DBF` (locally checked).
- **Installed version:** 1.0.0. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** retained candidate.
- **Purpose:** Compatibility patch adding ATAK support to the selected newer Minnesinger Gear family.
- **Direct external dependencies:** [Kova x ATAK](https://reforger.armaplatform.com/workshop/6A49417788E13618) (`6A49417788E13618`); [Minnesinger Gear](https://reforger.armaplatform.com/workshop/6A65091824BEC7DF) (`6A65091824BEC7DF`); [ATAK x COMPAT](https://reforger.armaplatform.com/workshop/6A756C77B915750B) (`6A756C77B915750B`)
- **Additional transitive dependencies:** [Zeliks Character](https://reforger.armaplatform.com/workshop/5D0551624969C92E) (`5D0551624969C92E`)
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Inspect and test before deciding on production inclusion.
- **Problems / compatibility:** Retained; vest-slot patch targeting the selected newer Gear. Requires Gear, Zeliks, Kova x ATAK and ATAK x COMPAT.

### 14. Customization, ranks, HUD and grenade limits

#### [Bacon Loadout Editor](https://reforger.armaplatform.com/workshop/606B100247F5C709)

- **Workshop ID:** `606B100247F5C709`. **Project GUID:** `606B100247F5C709` (locally checked).
- **Installed version:** 1.7.3. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** retained candidate.
- **Purpose:** Published page describes an editor for customizing Bacon M4 Block II and URG-I.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** Earlier handoff reports a failed save to a read-only original dependency config. This is not a gameplay failure or a successful NML whitelist test.
- **Recommendation:** Inspect and test before deciding on production inclusion.
- **Problems / compatibility:** Retained; loadout customization. Verify NML arsenal/rank access remains enforced through every inventory route.

#### [FTA Persistent Ranks](https://reforger.armaplatform.com/workshop/68A01393E2138094)

- **Workshop ID:** `68A01393E2138094`. **Project GUID:** `68A01393E2138094` (locally checked).
- **Installed version:** 1.0.28. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** member-selected.
- **Purpose:** Persists player XP/ranks across sessions and offers configurable wipe/restart behavior.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Retain for controlled testing as the member preference; production approval remains with Weston.
- **Problems / compatibility:** Selected; persistent XP/ranks. Test separately before deciding whether it replaces NML’s planned native progression.

#### [CERTII-Minimal-HUD](https://reforger.armaplatform.com/workshop/69CEF7D584AFD1FA)

- **Workshop ID:** `69CEF7D584AFD1FA`. **Project GUID:** `69CEF7D584AFD1FA` (locally checked).
- **Installed version:** 7.0.8. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** member-selected.
- **Purpose:** Minimal HUD that hides nametags and various XP/rank/stamina displays.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Retain for controlled testing as the member preference; production approval remains with Weston.
- **Problems / compatibility:** Selected; includes nametag removal and hides XP/rank/stamina/other displays. Check how FTA progression will be communicated.

#### [CERTII-Grenade-Limiter](https://reforger.armaplatform.com/workshop/69D92CDA0AE467E1)

- **Workshop ID:** `69D92CDA0AE467E1`. **Project GUID:** `69D92CDA0AE467E1` (locally checked).
- **Installed version:** 7.0.2. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** member-selected.
- **Purpose:** Limits grenade quantities through arsenal takes, pickups and spawn-load handling.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Retain for controlled testing as the member preference; production approval remains with Weston.
- **Problems / compatibility:** Selected; caps arsenal takes/pickups/spawn loads. Review against future NML native grenade rules.

### 15. Communications, vehicle security and administration

#### [Voice Volume Slider](https://reforger.armaplatform.com/workshop/647C19ACB69E7914)

- **Workshop ID:** `647C19ACB69E7914`. **Project GUID:** `647C19ACB69E7914` (locally checked).
- **Installed version:** 1.0.22. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** retained candidate.
- **Purpose:** Voice-volume controls and radio audio routing to ears.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Inspect and test before deciding on production inclusion.
- **Problems / compatibility:** Retained; voice volume and radio ear routing.

#### [WCS_SpawnProtection](https://reforger.armaplatform.com/workshop/614C00DA7F8765F6)

- **Workshop ID:** `614C00DA7F8765F6`. **Project GUID:** `614C00DA7F8765F6` (locally checked).
- **Installed version:** 8.1.0. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** retained candidate.
- **Purpose:** Spawn-protection system whose documented use includes Conflict HQs.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Inspect and test before deciding on production inclusion.
- **Problems / compatibility:** Needs mode testing; Conflict HQ protection is not yet verified in NML’s custom sandbox. Original WSC spelling corrected to WCS.

#### [WCS_VehicleLock](https://reforger.armaplatform.com/workshop/61BA4EB5C886D396)

- **Workshop ID:** `61BA4EB5C886D396`. **Project GUID:** `61BA4EB5C886D396` (locally checked).
- **Installed version:** 8.1.0. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** retained candidate.
- **Purpose:** Vehicle lock/unlock, shared keys and supply-protection features.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Inspect and test before deciding on production inclusion.
- **Problems / compatibility:** Retained; vehicle lock/unlock, shared keys and supply protection.

#### [GM Tools](https://reforger.armaplatform.com/workshop/64F10E068D5880A6)

- **Workshop ID:** `64F10E068D5880A6`. **Project GUID:** `64F10E068D5880A6` (locally checked).
- **Installed version:** 2.2.0. **Local status:** identity/payload/manifest-size checks passed.
- **Member status:** retained candidate.
- **Purpose:** Game Master/admin tools, permissions/ranks, cleanup and Discord relay/alerts.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Inspect and test before deciding on production inclusion.
- **Problems / compatibility:** Retained; administration/cleanup/Discord relay. Review rank/admin permissions separately from FTA progression.

#### [Discord Admin Stats](https://reforger.armaplatform.com/workshop/68C61B955A988F7E)

- **Workshop ID:** `68C61B955A988F7E`. **Project GUID:** unverified; project not found.
- **Installed version:** unknown. **Local status:** missing.
- **Member status:** retained candidate.
- **Purpose:** Discord-facing server statistics, kill/status feeds, administrative logs and chat relay.
- **Direct external dependencies:** None.
- **Additional transitive dependencies:** None.
- **Testing / working status:** No documented gameplay test; working/not-working status unknown.
- **Recommendation:** Inspect and test before deciding on production inclusion.
- **Problems / compatibility:** Retained; formerly unlabelled supplied link. Alerts/relay overlap GM Tools; configure one owner per feed to avoid duplicates. No proven hard conflict.

## Optional, parked and excluded packages — outside the 87

These are historical considered packages, not production approvals. Local project presence below is not a complete payload audit. All lack a documented gameplay result here; working status is unknown. Recommendations are to leave optional/parked packages outside the current set and keep excluded packages excluded unless the lead requests a new evaluation.

| Package / ID | Status | Local version / presence | Direct external dependencies | Reason / compatibility |
|---|---|---|---|---|
| [ChangeYourFace](https://reforger.armaplatform.com/workshop/65CEF81038D4AF3C) (`65CEF81038D4AF3C`) | Optional | Not found; installed version unknown | Not locally resolved | Face selection discussed but never selected; dependency evaluation incomplete. |
| [ACE Medical AI Dev](https://reforger.armaplatform.com/workshop/6A1AF6DD9938A580) (`6A1AF6DD9938A580`) | Optional | Not found; installed version unknown | Published: Medical Core Dev; Core Dev | AI treatment candidate; published requirements Medical Core Dev + Core Dev. Advanced medical coverage not established. |
| [AAO Team Balancer](https://reforger.armaplatform.com/workshop/9CBB2420AEA2D67F) (`9CBB2420AEA2D67F`) | Parked | 1.8.15; project found; GUID 9CBB2420AEA2D67F | None beyond vanilla | Check faction assumptions and balancing rules before reconsidering. |
| [Brutal Voices](https://reforger.armaplatform.com/workshop/6174A376E06661BF) (`6174A376E06661BF`) | Parked | 1.0.1; project found; GUID 6174A376E06661BF | None beyond vanilla | AI/radio/injury voices overlap parts of Screams of War; no runtime conflict proved. |
| [Lunacy Audio](https://reforger.armaplatform.com/workshop/64DE57C0A1601D14) (`64DE57C0A1601D14`) | Parked | 2.1.2; project found; GUID 64DE57C0A1601D14 | None beyond vanilla | Separate sound comparison; do not stack automatically. |
| [Realistic Combat Drones MANW](https://reforger.armaplatform.com/workshop/65EA144B050DC1F7) (`65EA144B050DC1F7`) | Parked | Not found; installed version unknown | Not locally resolved | Alternative to original drone package; previously published version 2.1.3. |
| [Minnesinger Mods Collection](https://reforger.armaplatform.com/workshop/67191670195CBA05) (`67191670195CBA05`) | Parked | 1.1.3; project found; GUID 67191670195CBA05 | [FORTEX Framework](https://reforger.armaplatform.com/workshop/66C2ED22F25B05C9) (`66C2ED22F25B05C9`); [Zeliks Character](https://reforger.armaplatform.com/workshop/5D0551624969C92E) (`5D0551624969C92E`) | Exact role unclear; not assumed to bundle newer family. |
| [Older Minnesinger Clothes](https://reforger.armaplatform.com/workshop/68CEC4AD24212E9F) (`68CEC4AD24212E9F`) | Parked | 1.0.8; project found; GUID 68CEC4AD24212E9F | [Zeliks Character](https://reforger.armaplatform.com/workshop/5D0551624969C92E) (`5D0551624969C92E`) | Newer family preferred. |
| [Older Minnesinger Core](https://reforger.armaplatform.com/workshop/68CEC5B536EB8B4A) (`68CEC5B536EB8B4A`) | Parked | 1.0.12; project found; GUID 68CEC5B536EB8B4A | [Zeliks Character](https://reforger.armaplatform.com/workshop/5D0551624969C92E) (`5D0551624969C92E`) | Newer family preferred. |
| [Older Minnesinger Gear](https://reforger.armaplatform.com/workshop/68CEC58B288B3AC0) (`68CEC58B288B3AC0`) | Parked | 1.0.10; project found; GUID 68CEC58B288B3AC0 | [Zeliks Character](https://reforger.armaplatform.com/workshop/5D0551624969C92E) (`5D0551624969C92E`); [FORTEX Framework](https://reforger.armaplatform.com/workshop/66C2ED22F25B05C9) (`66C2ED22F25B05C9`); [68CEC5B536EB8B4A](https://reforger.armaplatform.com/workshop/68CEC5B536EB8B4A) | Newer family preferred; earlier web-reader 404 did not establish local absence. |
| [Older Minnesinger Headwear](https://reforger.armaplatform.com/workshop/68CEC56636083A66) (`68CEC56636083A66`) | Parked | 1.0.5; project found; GUID 68CEC56636083A66 | [68CEC5B536EB8B4A](https://reforger.armaplatform.com/workshop/68CEC5B536EB8B4A); [Zeliks Character](https://reforger.armaplatform.com/workshop/5D0551624969C92E) (`5D0551624969C92E`) | Newer family preferred. |
| [Older Minnesinger Core x RHS compat](https://reforger.armaplatform.com/workshop/692D112F1B0B4072) (`692D112F1B0B4072`) | Parked | 1.0.6; project found; GUID 692D112F1B0B4072 | [68CEC58B288B3AC0](https://reforger.armaplatform.com/workshop/68CEC58B288B3AC0); [68CEC5B536EB8B4A](https://reforger.armaplatform.com/workshop/68CEC5B536EB8B4A) | Targets older Core/Gear; need not assumed for newer family. |
| [SlavicWar Bodycam POV System](https://reforger.armaplatform.com/workshop/6C7A53E2B9F104D8) (`6C7A53E2B9F104D8`) | Excluded | Not found; installed version unknown | None listed on published page; local project unavailable | Torso/head POV; previously published version 1.0.0. Temporary comparison idea cancelled; retain CERTII + Best Body Cam. No local test. |
| [ACE Medical Red Pain](https://reforger.armaplatform.com/workshop/6564C717166435A9) (`6564C717166435A9`) | Excluded | Not found; installed version unknown | Not locally resolved | Official Dev medical preferred over older third-party copy. |
| [Regular ACE Medical Core](https://reforger.armaplatform.com/workshop/60C4C12DAE90727B) (`60C4C12DAE90727B`) | Excluded | Not found; installed version unknown | Not locally resolved | Matching Dev channel chosen. |
| [Regular ACE Medical Hitzones](https://reforger.armaplatform.com/workshop/65860252A17554C7) (`65860252A17554C7`) | Excluded | Not found; installed version unknown | Not locally resolved | Matching Dev channel chosen. |
| [EE Suppression](https://reforger.armaplatform.com/workshop/665C00EFCCAE1AB3) (`665C00EFCCAE1AB3`) | Excluded | 1.0.5; project found; GUID 665C00EFCCAE1AB3 | None beyond vanilla | GC chosen. |
| [LM Suppression](https://reforger.armaplatform.com/workshop/613B4DD4A91CA144) (`613B4DD4A91CA144`) | Excluded | 1.8.0; project found; GUID 613B4DD4A91CA144 | None beyond vanilla | GC chosen. |
| [ACE Overheating Dev](https://reforger.armaplatform.com/workshop/65B8EA178A3E94E3) (`65B8EA178A3E94E3`) | Excluded | Not found; installed version unknown | Not locally resolved | 2-7 chosen. |
| [Realism Overhaul - Sounds](https://reforger.armaplatform.com/workshop/631B695913C7781F) (`631B695913C7781F`) | Excluded | 1.8.1; project found; GUID 631B695913C7781F | None beyond vanilla | Realistic Combat Sound Mod chosen; recorded author earplug incompatibility. |
| [SR Sound Mod](https://reforger.armaplatform.com/workshop/605F3615AB65443C) (`605F3615AB65443C`) | Excluded | 0.8.141; project found; GUID 605F3615AB65443C | None beyond vanilla | Realistic Combat Sound Mod chosen. |
| [Death sound effect](https://reforger.armaplatform.com/workshop/69D4B8A05C764676) (`69D4B8A05C764676`) | Excluded | 1.0.1; project found; GUID 69D4B8A05C764676 | None beyond vanilla | Screams of War chosen. |
| [Realistic Explosion Concussion](https://reforger.armaplatform.com/workshop/68CAB06687F2BAF7) (`68CAB06687F2BAF7`) | Excluded | 1.0.3; project found; GUID 68CAB06687F2BAF7 | [59EAA899751805DF](https://reforger.armaplatform.com/workshop/59EAA899751805DF); [68CB6ECFED8513A2](https://reforger.armaplatform.com/workshop/68CB6ECFED8513A2) | Requires Pizzas Tinnitus and Stun Grenade, transitively excluded EE. |
| [Pizzas Explosion Tinnitus](https://reforger.armaplatform.com/workshop/68CB6ECFED8513A2) (`68CB6ECFED8513A2`) | Excluded | 1.0.2; project found; GUID 68CB6ECFED8513A2 | [665C00EFCCAE1AB3](https://reforger.armaplatform.com/workshop/665C00EFCCAE1AB3) | Requires excluded EE. |
| [Extra Eastern Grenades](https://reforger.armaplatform.com/workshop/68A8DD0FEBA513DB) (`68A8DD0FEBA513DB`) | Excluded | 1.0.2; project found; GUID 68A8DD0FEBA513DB | None beyond vanilla | Content duplicated by selected Extra Grenades. |
| [Extra Improvised Grenades](https://reforger.armaplatform.com/workshop/68A7689CFF67E664) (`68A7689CFF67E664`) | Excluded | 1.0.2; project found; GUID 68A7689CFF67E664 | None beyond vanilla | Content duplicated by selected Extra Grenades. |
| [Immersive Head Movement](https://reforger.armaplatform.com/workshop/6622D3D1E5A3809D) (`6622D3D1E5A3809D`) | Excluded | 1.5.1; project found; GUID 6622D3D1E5A3809D | None beyond vanilla | CERTII movement chosen; do not add another camera layer automatically. |
| [No Player Name Tags](https://reforger.armaplatform.com/workshop/64D5438A385B978C) (`64D5438A385B978C`) | Excluded | 0.9.91; project found; GUID 64D5438A385B978C | None beyond vanilla | Selected CERTII HUD supplies nametag hiding. |
| [Bacon Arsenal Only](https://reforger.armaplatform.com/workshop/66D541B6D0B189ED) (`66D541B6D0B189ED`) | Optional / deferred | Not found; installed version unknown | Not locally resolved | Do not add before base arsenal and Bacon interactions are understood. |
| [IRBA](https://reforger.armaplatform.com/workshop/94349117F380C47C) (`94349117F380C47C`) | Optional / deferred | Not found; installed version unknown | Not locally resolved | Catalog compatibility concept from earlier handoff; actual dependency/integration evaluation incomplete. |
| [Stun Grenade](https://reforger.armaplatform.com/workshop/59EAA899751805DF) (`59EAA899751805DF`) | Excluded-chain dependency | 1.3.0; project found; GUID 59EAA899751805DF | None beyond vanilla | Required by excluded Concussion; not selected for the 87-package set. |

Other locally discovered SlavicWar projects and Myrove are documented in the [arsenal/resource handoff](2026-10-02-arsenal-status.md). They are not silently added to this inventory.

## Evidence limitations and remaining checks

No combined medical, attachment, sound, recoil, camera, vehicle, persistence or multiplayer compatibility certificate exists. Author claims are not test results. Package download presence is not successful registration in Workbench and not a server deployment.

Specific pending checks include FORTEX/2-7 heating, ACE/CERTII/GC/ragdoll/weapon-retention interactions, thermal scope restrictions and faction replacement, old-game-target weapon compatibility, Deluxe effects performance, spawn protection in a custom sandbox, and Bacon whitelist enforcement. See the handoff for the complete lead-decision list.
