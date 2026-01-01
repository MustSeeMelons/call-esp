import { useRef, useState } from "react";
import "./App.scss";
import { Frame, type IFrameApi } from "./components/frame/frame";
import { ThemeProvider } from "./context/theme-context";
import { Letters } from "./components/letters/letters";
import { saveAsJson, SMOL_DELAY } from "./utils";
import { v4 } from "uuid";
import React from "react";
import { FileSelect } from "./components/file-select/file-select";

interface IJsonConfig {
  results: number[][];
}

function App() {
  const [fileName, setFileName] = useState("frames.json");

  const [frames, setFrames] = useState<{ id: string }[]>([{ id: v4() }]);
  const frameApis = useRef(
    new Map<string, React.RefObject<IFrameApi | null>>()
  );

  const getFrameApi = (id: string) => {
    if (!frameApis.current.has(id)) {
      frameApis.current.set(id, React.createRef<IFrameApi | null>());
    }

    return frameApis.current.get(id)!;
  };

  return (
    <ThemeProvider>
      <h1>Letters</h1>
      <Letters />
      <h1
        style={{
          display: "flex",
          alignItems: "center",
          justifyContent: "start",
          columnGap: "10px",
        }}
      >
        Frame Composer
        {/* XXX Custom component, so we can have clear */}
        <input
          style={{ width: "150px", margin: "0" }}
          type="text"
          value={fileName}
          onChange={(e) => setFileName(e.target.value)}
        />
        <button
          onClick={() => {
            const results: number[][] = [];

            frameApis.current.forEach((f) => {
              const config = f.current?.getData();
              if (config) {
                results.push(config);
              }
            });

            const json = { results: results };

            saveAsJson(json, fileName);
          }}
        >
          save
        </button>
        <FileSelect
          labelNode="load"
          onChange={async (file) => {
            setFileName(file.name);

            const data = await file.text();

            const object = JSON.parse(data) as IJsonConfig;

            setFrames([]);
            frameApis.current.clear();

            object.results.forEach((frame) => {
              const id = v4();

              setFrames((p) => [...p, { id }]);
              const api = getFrameApi(id);

              setTimeout(() => {
                api.current?.setData(frame);
              }, SMOL_DELAY);
            });
          }}
        />
      </h1>
      <p>Do a counter-clock-wise rotation when done!</p>
      <div style={{ rowGap: "20px", display: "flex", flexDirection: "column" }}>
        {frames.map((f, idx) => {
          return (
            <Frame
              key={f.id}
              api={getFrameApi(f.id)}
              deleteCallback={() => {
                setFrames((p) => p.filter((v) => v.id !== f.id));
                frameApis.current.delete(f.id);
              }}
              moveBefore={() => {
                if (frames.length < 2) {
                  return;
                }

                setFrames((p) => {
                  const c = [...p];

                  const target = (idx - 1 + frames.length) % frames.length;

                  [c[idx], c[target]] = [c[target], c[idx]];

                  return c;
                });
              }}
              moveAfer={() => {
                if (frames.length < 2) {
                  return;
                }

                setFrames((p) => {
                  const c = [...p];

                  const target = (idx + 1 + frames.length) % frames.length;

                  [c[target], c[idx]] = [c[idx], c[target]];

                  return c;
                });
              }}
            />
          );
        })}
      </div>
      <div className="frame-controls">
        <button onClick={() => setFrames((p) => [...p, { id: v4() }])}>
          add frame
        </button>
        <button
          onClick={async () => {
            const data = await navigator.clipboard.readText();

            if (!data) {
              return;
            }

            const nums = data.split(",");

            if (nums.length !== 8) {
              return;
            }

            const nextId = v4();
            setFrames((p) => [...p, { id: nextId }]);

            setTimeout(async () => {
              frameApis.current
                .get(nextId)
                ?.current?.setData(nums.map((n) => Number(n)));
            }, SMOL_DELAY);
          }}
        >
          paste frame
        </button>
      </div>
    </ThemeProvider>
  );
}

export default App;
