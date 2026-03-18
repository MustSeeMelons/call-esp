import { PIXEL_COUNT } from "./definitions";

export const saveAsJson = (data: unknown, filename = "frames.json") => {
  const json = JSON.stringify(data, null, 2);
  const blob = new Blob([json], { type: "application/json" });
  const url = URL.createObjectURL(blob);

  const link = document.createElement("a");
  link.href = url;
  link.download = (() => {
    if (!filename.endsWith(".json")) {
      filename = `${filename}.json`;
    }

    return filename;
  })();

  link.click();

  URL.revokeObjectURL(url);
};

export const SMOL_DELAY = 50;

export const rotateArrayClockwise = (arr: number[]) => {
  const newArr = new Array(PIXEL_COUNT).fill(0);

  for (let row = 0; row < PIXEL_COUNT; row++) {
    for (let col = 0; col < PIXEL_COUNT; col++) {
      const bit = (arr[row] >> col) & 1;

      if (bit) {
        // The first row is the last column, the last row is the first column
        newArr[col] |= 1 << (7 - row);
      }
    }
  }

  return newArr;
};

export const rotateArrayCounterClockwise = (arr: number[]) => {
  const newArr = new Array(PIXEL_COUNT).fill(0);

  for (let row = 0; row < PIXEL_COUNT; row++) {
    for (let col = 0; col < PIXEL_COUNT; col++) {
      const bit = (arr[row] >> col) & 1;

      if (bit) {
        // The last column becomes the first row
        newArr[7 - col] |= 1 << row;
      }
    }
  }

  return newArr;
};
