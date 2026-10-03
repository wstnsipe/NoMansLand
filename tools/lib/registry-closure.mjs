// Shared helpers for the dependency registry (dependencies/mods.json).
// Used by tools/validate.mjs and tools/modlist.mjs. Zero dependencies, Node >= 18.

export const GUID_RE = /^[0-9A-F]{16}$/;

// `requires` / `requiredBy` are arrays of 16-hex GUIDs; a comma separated string is tolerated.
// Returns upper-cased, de-duplicated, sorted ids (empty entries dropped).
export function parseIds(value)
{
	if (value === undefined || value === null || value === "")
		return [];
	const raw = Array.isArray(value) ? value : String(value).split(",");
	return [...new Set(raw.map((v) => String(v).trim().toUpperCase()).filter((v) => v !== ""))].sort();
}

// Dependency closure of `roots` over the `requires` edges of `registry` (Map modId -> entry).
// order: every reachable registered mod, dependencies before dependents, deterministic
//        (roots and requires are visited in ascending modId order).
// missing: [{ id, by }] required ids that are not registered (`by` is the first requirer seen).
// cycles: [[id, ...]] dependency cycles found.
export function closure(registry, roots)
{
	const order = [];
	const missing = [];
	const cycles = [];
	const state = new Map();
	const stack = [];
	const visit = (id, by) =>
	{
		if (!registry.has(id))
		{
			if (!missing.some((m) => m.id === id))
				missing.push({ id, by });
			return;
		}
		if (state.get(id) === "done")
			return;
		if (state.get(id) === "visiting")
		{
			cycles.push([...stack.slice(stack.indexOf(id)), id]);
			return;
		}
		state.set(id, "visiting");
		stack.push(id);
		for (const dep of parseIds(registry.get(id).requires))
			visit(dep, id);
		stack.pop();
		state.set(id, "done");
		order.push(id);
	};
	for (const root of [...new Set(roots.map((r) => String(r).toUpperCase()))].sort())
		visit(root, null);
	return { order, missing, cycles };
}
