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


export const paramBuilder = (loadOptions: LoadOptions) => {
    
    const filters = loadOptions.filter as Array<FilterDescriptor>;
    const params: Record<string, any> = {}
    const isAndFilters = filters?.find(item => item === 'and');
    if (isAndFilters && filters) {
        const flattendFilters = filters.flat()
        flattendFilters.forEach(f => {
            params[f[0]] = f[2];
        })
    } else if (filters && isAllElmArray(filters)){
        filters.forEach(f => {
            params[f[0]] = f[2];
        })
    } else {
        params[filters[0]] = filters[2]
    }

    return params;
}