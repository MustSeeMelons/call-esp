import { useEffect, useState } from "react";
import "./frame.scss";

const PIXEL_COUNT = 8;

export interface IFrameApi {
  getData: () => number[];
  setData: (data: number[]) => void;
}

export interface IFrameProps {
  api?: React.RefObject<IFrameApi | null>;
  deleteCallback: () => void;
  moveBefore: () => void;
  moveAfer: () => void;
}

export const Frame: React.FC<IFrameProps> = ({
  api,
  deleteCallback,
  moveAfer,
  moveBefore,
}) => {
  const [frameRows, setFrameRows] = useState<number[]>(
    new Array(PIXEL_COUNT).fill(0)
  );

  const [isMouseDown, setMouseDown] = useState(false);

  // XXX move this into a seperate hook so each frame does not add listeners
  useEffect(() => {
    const onMouseDown = () => setMouseDown(true);
    const onMouseUp = () => setMouseDown(false);

    document.addEventListener("mousedown", onMouseDown);
    document.addEventListener("mouseup", onMouseUp);

    return () => {
      document.removeEventListener("mousedown", onMouseDown);
      document.removeEventListener("mouseup", onMouseUp);
    };
  }, []);

  useEffect(() => {
    if (!api) {
      return;
    }

    api.current = {
      getData: () => frameRows,
      setData: (data) => setFrameRows(data),
    };
  });

  const renderPixels = (value: number, rowIdx: number) => {
    const results: React.ReactNode[] = [];

    for (let i = 0; i < PIXEL_COUNT; i++) {
      const v = (value >> i) & 1;

      const setHandler = (currRows: number[]) => {
        return currRows.map((currRow, currRowIdx) => {
          if (currRowIdx === rowIdx) {
            if (v === 0) {
              const mask = 1 << i;
              return currRow | mask;
            } else {
              const mask = ~(1 << i);
              return currRow & mask;
            }
          } else {
            return currRow;
          }
        });
      };

      results.push(
        <div
          key={`${rowIdx}-${value}-${i}`}
          className={`pixel${v ? " pixel-active" : ""}`}
          onMouseDown={() => setFrameRows(setHandler)}
          onMouseEnter={() => {
            if (isMouseDown) {
              setFrameRows(setHandler);
            }
          }}
          onClick={() => {
            setFrameRows(setHandler);
          }}
        />
      );
    }

    return results;
  };

  const shiftLeft = () => {
    setFrameRows((currRows) => currRows.map((row) => row >> 1));
  };

  const shiftDown = () => {
    setFrameRows((currRows) => {
      let newFrameRows = [...currRows];
      newFrameRows.splice(currRows.length - 1, 1);
      newFrameRows = [0, ...newFrameRows];
      return newFrameRows;
    });
  };

  const shiftUp = () => {
    setFrameRows((currRows) => {
      const newFrameRows = [...currRows];
      newFrameRows.splice(0, 1);
      newFrameRows.push(0);
      return newFrameRows;
    });
  };

  const shiftRight = () => {
    setFrameRows((currRows) => currRows.map((row) => row << 1));
  };

  const clockWise = () => {
    const newRows = new Array(PIXEL_COUNT).fill(0);

    for (let row = 0; row < PIXEL_COUNT; row++) {
      for (let col = 0; col < PIXEL_COUNT; col++) {
        const bit = (frameRows[row] >> col) & 1;

        if (bit) {
          // The first row is the last column, the last row is the first column
          newRows[col] |= 1 << (7 - row);
        }
      }
    }

    return setFrameRows(newRows);
  };

  const counterClockWise = () => {
    const newRows = new Array(PIXEL_COUNT).fill(0);

    for (let row = 0; row < PIXEL_COUNT; row++) {
      for (let col = 0; col < PIXEL_COUNT; col++) {
        const bit = (frameRows[row] >> col) & 1;

        if (bit) {
          // The last column becomes the first row
          newRows[7 - col] |= 1 << row;
        }
      }
    }

    return setFrameRows(newRows);
  };

  return (
    <div className="container">
      <div className="frame">
        {frameRows.map((row, idx) => {
          return (
            <div key={`${row}-${idx}`} className="row">
              {renderPixels(row, idx)}
            </div>
          );
        })}
      </div>
      <div className="controls">
        <button onClick={shiftUp}>shift up</button>
        <button onClick={shiftDown}>shift down</button>
        <button onClick={shiftLeft}>shift left</button>
        <button onClick={shiftRight}>shift right</button>
        <button className="gc2" onClick={clockWise}>
          clockwise
        </button>
        <button className="gc2" onClick={counterClockWise}>
          counter
        </button>
        <button
          className="gc3"
          onClick={() => navigator.clipboard.writeText(frameRows.join(", "))}
        >
          copy
        </button>
        <button
          className="gc3"
          onClick={() => setFrameRows(new Array(PIXEL_COUNT).fill(0))}
        >
          clear
        </button>
        <button className="gc3" onClick={() => deleteCallback()}>
          delete
        </button>
        <button
          className="gc4"
          onClick={(e) => {
            e.currentTarget.blur();
            moveBefore();
          }}
        >
          before
        </button>
        <button
          className="gc4"
          onClick={(e) => {
            e.currentTarget.blur();
            moveAfer();
          }}
        >
          after
        </button>
      </div>
      <div className="data">{frameRows.join(", ")}</div>
    </div>
  );
};
