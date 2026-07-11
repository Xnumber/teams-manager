import { Pipe, PipeTransform } from '@angular/core';

@Pipe({
  name: 'changeLine',
})
export class ChangeLinePipe implements PipeTransform {
  private static readonly htmlEscapeMap: Record<string, string> = {
    '&': '&amp;',
    '<': '&lt;',
    '>': '&gt;',
    '"': '&quot;',
    "'": '&#39;',
  };

  transform(value: string | null | undefined): string {
    if (!value) {
      return '';
    }

    const escapedValue = value.replace(/[&<>"']/g, (char) => ChangeLinePipe.htmlEscapeMap[char]);

    return escapedValue.replace(/\r?\n/g, '<br>');
  }

}
