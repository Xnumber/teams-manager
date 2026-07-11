import { LoadOptions } from 'devextreme/data';

type FilterCondition = [string, string, any];

function isFilterCondition(node: unknown): node is FilterCondition {
  return (
    Array.isArray(node) &&
    node.length === 3 &&
    typeof node[0] === 'string' &&
    typeof node[1] === 'string'
  );
}

export function conditionsToObject(
  conditions: LoadOptions['filter']
): Record<string, any> {
  const result: Record<string, any> = {};

  const traverse = (node: unknown): void => {
    if (!Array.isArray(node)) return;

    if (isFilterCondition(node)) {
      const [field, operator, value] = node;

      if (operator === '=') {
        result[field] = value;
      }

      if (operator === 'contains') {
        result[field] = value;
      }

      return;
    }

    node.forEach(traverse);
  };

  traverse(conditions);

  return result;
}