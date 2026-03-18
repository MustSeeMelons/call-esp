import { useEffect, useState } from "react";
import "./frame.scss";
import { PIXEL_COUNT } from "../../definitions";
import { rotateArrayClockwise, rotateArrayCounterClockwise } from "../../utils";
import { useMouseDown } from "../../hooks/useMouseDown";

export interface IFrameApi {
  getData: () => number[];
  setData: (data: number[]) => void;
  rotateClockwise: () => void;
  rotateCounterClockwise: () => void;
}

export interface IFrameProps {
  api?: React.RefObject<IFrameApi | null>;
  deleteCallback: () => void;
  moveBefore: () => void;
  moveAfter: () => void;
}

export const Frame: React.FC<IFrameProps> = ({
  api,
  deleteCallback,
  moveAfter,
  moveBefore,
}) => {
  const [frameRows, setFrameRows] = useState<number[]>(
    new Array(PIXEL_COUNT).fill(0),
  );

  const isMouseDown = useMouseDown();

  useEffect(() => {
    if (!api) {
      return;
    }

    api.current = {
      getData: () => frameRows,
      setData: (data) => setFrameRows(data),
      rotateClockwise: () => setFrameRows(rotateArrayClockwise(frameRows)),
      rotateCounterClockwise: () =>
        setFrameRows(rotateArrayCounterClockwise(frameRows)),
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
        />,
      );
    }

    return results;
  };

  const shiftLeft = () => {
    setFrameRows((currRows) => currRows.map((row) => (row >> 1) & 0xff));
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
    setFrameRows((currRows) => currRows.map((row) => (row << 1) & 0xff));
  };

  const clockWise = () => {
    return setFrameRows(rotateArrayClockwise(frameRows));
  };

  const counterClockWise = () => {
    return setFrameRows(rotateArrayCounterClockwise(frameRows));
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
            moveAfter();
          }}
        >
          after
        </button>
      </div>
      <div className="data">{frameRows.join(", ")}</div>
    </div>
  );
};
