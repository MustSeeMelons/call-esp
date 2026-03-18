import { useEffect, useState } from "react";
import "./playback.scss";
import { PIXEL_COUNT } from "../../definitions";
import { useSessionStorage } from "hooks-ts";
import { rotateArrayClockwise, rotateArrayCounterClockwise } from "../../utils";

const DEF_COL_GAP = 5;
const DEF_ROW_GAP = 5;

interface IPlaybackProps {
  frames: number[][];
}

export const Playback: React.FC<IPlaybackProps> = ({ frames }) => {
  const [currFrames, setCurrFrames] = useState(frames);

  const [frameCount] = useState(frames.length);
  const [frameIdx, setFrameIdx] = useState(0);
  const [frameDelay, setFrameDelay] = useSessionStorage<number | "">(
    "frameDelay",
    100,
  ); // In ms
  const [stretchX, setStretchX] = useSessionStorage<number | "">("stretchX", 0);
  const [stretchY, setStretchY] = useSessionStorage<number | "">("stretchY", 0);
  const [isPingPong, setPingPong] = useSessionStorage("isPingPong", false);
  const [isForward, setForward] = useSessionStorage("isForward", false);

  useEffect(() => {
    if (frameDelay === "") {
      return;
    }

    const timeout = setTimeout(() => {
      if (!isPingPong) {
        setFrameIdx((idx) => (idx + 1) % frameCount);
        return;
      }

      if (isForward) {
        const nextIdx = frameIdx + 1;
        if (nextIdx >= frameCount) {
          setForward(false);
          setFrameIdx(nextIdx - 1);
        } else {
          setFrameIdx(nextIdx);
        }
      } else {
        const nextIdx = frameIdx - 1;
        if (nextIdx < 0) {
          setForward(true);
          setFrameIdx(nextIdx + 1);
        } else {
          setFrameIdx(nextIdx);
        }
      }
    }, frameDelay);

    return () => clearTimeout(timeout);
  }, [frameDelay, isPingPong, isForward, frameIdx]);

  const renderPixels = () => {
    const frameData = currFrames[frameIdx];
    const results: React.ReactNode[] = [];

    for (let row = 0; row < PIXEL_COUNT; row++) {
      const rowResults: React.ReactNode[] = [];

      for (let col = 0; col < PIXEL_COUNT; col++) {
        const v = (frameData[row] >> col) & 1;

        rowResults.push(
          <div
            key={`${frameIdx}-${col}`}
            className={`playback-pixel${v ? " pixel-active" : ""}`}
          />,
        );
      }

      results.push(
        <div
          key={`${frameIdx}-row-${row}`}
          className="row"
          style={{ columnGap: `${DEF_COL_GAP + (stretchX || 0)}px` }}
        >
          {rowResults}
        </div>,
      );
    }

    return results;
  };

  const numberOnChangeHandler = (
    e: React.ChangeEvent<HTMLInputElement>,
    setValue: (value: number | "") => void,
    options?: { min?: number; max?: number },
  ) => {
    const val = e.target.value;

    if (val === "") {
      setValue("");
      return;
    }

    const num = parseInt(val, 10);
    if (!isNaN(num)) {
      const clamped = Math.max(
        options?.min ?? 1,
        Math.min(options?.max ?? 5000, num),
      );
      setValue(clamped);
    }
  };

  const numberBlurHandler = (
    e: React.ChangeEvent<HTMLInputElement>,
    setValue: (value: number | "") => void,
  ) => {
    if (e.target.value === "" || isNaN(Number(e.target.value))) {
      setValue(1);
    }
  };

  return (
    <div className="playback">
      <h1>Playback</h1>
      <div className="tools">
        <div
          className="frame"
          style={{ rowGap: `${DEF_ROW_GAP + (stretchY || 0)}px` }}
        >
          {renderPixels()}
        </div>
        <div style={{ flexGrow: 1 }}>
          <div className="form-control">
            <label htmlFor="frame-delay">Frame Delay:</label>
            <input
              id="frame-delay"
              type="number"
              min="1"
              max="5000"
              step="1"
              value={frameDelay}
              onChange={(e) => {
                numberOnChangeHandler(e, setFrameDelay, { min: 1, max: 5000 });
              }}
              onBlur={(e) => {
                numberBlurHandler(e, setFrameDelay);
              }}
            />
          </div>
          <div className="form-control">
            <label htmlFor="stretch-x">Stretch X:</label>
            <input
              id="stretch-x"
              type="number"
              min="1"
              max="5000"
              step="1"
              value={stretchX}
              onChange={(e) => {
                numberOnChangeHandler(e, setStretchX, { min: 1, max: 5000 });
              }}
              onBlur={(e) => {
                numberBlurHandler(e, setStretchX);
              }}
            />
          </div>
          <div className="form-control">
            <label htmlFor="stretch-y">Stretch Y:</label>
            <input
              id="stretch-y"
              type="number"
              min="1"
              max="5000"
              step="1"
              value={stretchY}
              onChange={(e) => {
                numberOnChangeHandler(e, setStretchY, { min: 1, max: 5000 });
              }}
              onBlur={(e) => {
                numberBlurHandler(e, setStretchY);
              }}
            />
          </div>
          <div className="form-control">
            <label htmlFor="ping-pong">Ping Pong:</label>
            <input
              id="ping-pong"
              type="checkbox"
              checked={isPingPong}
              onChange={(e) => {
                setPingPong(e.target.checked);
              }}
            />
          </div>
          <div className="form-control" style={{ columnGap: "1rem" }}>
            <button
              onClick={() => {
                setCurrFrames((curr) => curr.map(rotateArrayCounterClockwise));
              }}
            >
              counter
            </button>
            <button
              onClick={() => {
                setCurrFrames((curr) => curr.map(rotateArrayClockwise));
              }}
            >
              clock
            </button>
          </div>
        </div>
      </div>
    </div>
  );
};
