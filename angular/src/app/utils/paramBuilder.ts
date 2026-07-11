import { FilterDescriptor, LoadOptions } from "devextreme/data";

const isAllElmArray = (filter: any[]) => {
    let isAllArray = true

    const isArray = Array.isArray(filter)

    if(!isArray) {
        return false;
    }

    filter.forEach(f => {
        if (!Array.isArray(f)) {
            isAllArray = false
        }
    })

    return isAllArray
}

function extractFilterConditions(filters: Array<FilterDescriptor>): Array<FilterDescriptor> {
  const result: Array<FilterDescriptor> = [];

  function traverse(node: Array<FilterDescriptor>) {
    if (!Array.isArray(node)) return;

    // 是一個條件，例如 ["field", "=", "value"]
    if (
      node.length === 3 &&
      typeof node[0] === "string" &&
      typeof node[1] === "string"
    ) {
      result.push(node);
      return;
    }

    // 繼續往下找
    for (const item of node) {
      traverse(item);
    }
  }

  traverse(filters);
  return result;
}

export const paramBuilder = (loadOptions: LoadOptions) => {
    const resultFilter = [];
    const filters = loadOptions.filter as Array<FilterDescriptor>;
    
    const flattendfilters = extractFilterConditions(filters)
    
    const params: Record<string, any> = {}
    // console.log(filters)
    // const isAndFilters = filters?.find(item => item === 'and');
    
    flattendfilters.forEach(f => {
        params[f[0]] = f[2];
    })

    return params;
}