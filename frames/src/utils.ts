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
