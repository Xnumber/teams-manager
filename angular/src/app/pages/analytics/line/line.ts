import { Component, Input, AfterViewInit, ViewChild, ElementRef } from '@angular/core';

@Component({
  selector: 'app-line',
  templateUrl: './line.html',
  styleUrls: ['./line.scss'],
})
export class Line implements AfterViewInit {
  @Input() data: Array<{ x: string, y: number }> = [];
  @ViewChild('lineCanvas', { static: true }) canvasRef!: ElementRef<HTMLCanvasElement>;
  PAD = { top: 20, right: 24, bottom: 36, left: 52 };
  BLUE_FILL = 'rgba(55,138,221,0.25)';
  BLUE_LINE = '#185FA5';
  RED_FILL = 'rgba(226,75,74,0.25)';
  RED_LINE = '#A32D2D';
  GRID = 'rgba(136,135,128,0.18)';
  AXIS_TEXT = '#888780';
  ngAfterViewInit() {
    this.drawLineChart();
  }

  ngOnChanges() {
    this.drawLineChart();
  }

  drawLineChart() {
    const canvas = this.canvasRef.nativeElement;
    const container = canvas.parentElement!;
    const W = container.clientWidth;
    const H = Math.round(W * 0.42);
    canvas.width = W;
    canvas.height = H;
    const ctx = canvas.getContext('2d');
    if (!ctx) return;

    const { PAD } = this;
    ctx.clearRect(0, 0, W, H);

    const cW = W - PAD.left - PAD.right;
    const cH = H - PAD.top - PAD.bottom;

    const yMax = Math.max(...this.data.map(d => d.y)) * 1.2;
    const yMin = Math.min(...this.data.map(d => d.y)) * 1.2;
    const yRange = yMax - yMin;

    const xPos = (i: number) => PAD.left + (i / (this.data.length - 1)) * cW;
    const yPos = (v: number) => PAD.top + (1 - (v - yMin) / yRange) * cH;

    const zeroY = yPos(0);

    // grid lines
    const ticks = 5;
    for (let i = 0; i <= ticks; i++) {
      const v = yMin + (yRange / ticks) * i;
      const y = yPos(v);
      ctx.strokeStyle = this.GRID;
      ctx.lineWidth = 0.5;
      ctx.beginPath();
      ctx.moveTo(PAD.left, y);
      ctx.lineTo(PAD.left + cW, y);
      ctx.stroke();
      ctx.fillStyle = this.AXIS_TEXT;
      ctx.font = '11px sans-serif';
      ctx.textAlign = 'right';
      ctx.fillText(Math.round(v).toString(), PAD.left - 8, y + 4);
    }

    // zero line
    ctx.strokeStyle = 'rgba(136,135,128,0.5)';
    ctx.lineWidth = 1;
    ctx.setLineDash([4, 3]);
    ctx.beginPath();
    ctx.moveTo(PAD.left, zeroY);
    ctx.lineTo(PAD.left + cW, zeroY);
    ctx.stroke();
    ctx.setLineDash([]);

    // x axis labels
    ctx.fillStyle = this.AXIS_TEXT;
    ctx.font = '11px sans-serif';
    ctx.textAlign = 'center';
    this.data.forEach((d, i) => {
      ctx.fillText(d.x, xPos(i), H - 8);
    });

    // clipping + fill for positive (above zero)
    ctx.save();
    ctx.beginPath();
    ctx.rect(PAD.left, PAD.top, cW, zeroY - PAD.top);
    ctx.clip();
    ctx.beginPath();
    ctx.moveTo(xPos(0), yPos(this.data[0].y));
    this.data.forEach((d, i) => ctx.lineTo(xPos(i), yPos(d.y)));
    ctx.lineTo(xPos(this.data.length - 1), zeroY);
    ctx.lineTo(xPos(0), zeroY);
    ctx.closePath();
    ctx.fillStyle = this.BLUE_FILL;
    ctx.fill();
    ctx.restore();

    // clipping + fill for negative (below zero)
    ctx.save();
    ctx.beginPath();
    ctx.rect(PAD.left, zeroY, cW, PAD.top + cH - zeroY);
    ctx.clip();
    ctx.beginPath();
    ctx.moveTo(xPos(0), yPos(this.data[0].y));
    this.data.forEach((d, i) => ctx.lineTo(xPos(i), yPos(d.y)));
    ctx.lineTo(xPos(this.data.length - 1), zeroY);
    ctx.lineTo(xPos(0), zeroY);
    ctx.closePath();
    ctx.fillStyle = this.RED_FILL;
    ctx.fill();
    ctx.restore();

    // draw line segments colored by value
    for (let i = 0; i < this.data.length - 1; i++) {
      const x0 = xPos(i), y0 = yPos(this.data[i].y);
      const x1 = xPos(i + 1), y1 = yPos(this.data[i + 1].y);
      const v0 = this.data[i].y, v1 = this.data[i + 1].y;

      if ((v0 >= 0 && v1 >= 0) || (v0 <= 0 && v1 <= 0)) {
        ctx.strokeStyle = v0 >= 0 ? this.BLUE_LINE : this.RED_LINE;
        ctx.lineWidth = 2;
        ctx.beginPath();
        ctx.moveTo(x0, y0);
        ctx.lineTo(x1, y1);
        ctx.stroke();
      } else {
        // crossing zero — split
        const t = Math.abs(v0) / (Math.abs(v0) + Math.abs(v1));
        const xMid = x0 + (x1 - x0) * t;
        ctx.lineWidth = 2;

        ctx.strokeStyle = v0 >= 0 ? this.BLUE_LINE : this.RED_LINE;
        ctx.beginPath(); ctx.moveTo(x0, y0); ctx.lineTo(xMid, zeroY); ctx.stroke();

        ctx.strokeStyle = v1 >= 0 ? this.BLUE_LINE : this.RED_LINE;
        ctx.beginPath(); ctx.moveTo(xMid, zeroY); ctx.lineTo(x1, y1); ctx.stroke();
      }
    }

    // dots
    this.data.forEach((d, i) => {
      ctx.beginPath();
      ctx.arc(xPos(i), yPos(d.y), 4, 0, Math.PI * 2);
      ctx.fillStyle = d.y >= 0 ? this.BLUE_LINE : this.RED_LINE;
      ctx.fill();
      ctx.strokeStyle = '#fff';
      ctx.lineWidth = 1.5;
      ctx.stroke();
    });
  }
}

