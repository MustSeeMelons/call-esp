import { useState } from "react";
import "./letters.scss";

export const Letters = () => {
  const [combination, setCombination] = useState(0);

  return (
    <div className="letters">
      <div className="from-control">
        <label htmlFor="offset">Offset:</label>
        <input
          id="offset"
          type="text"
          value={combination & 0xf}
          onChange={(e) => {
            const offset = Number(e.target.value);

            setCombination((p) => {
              const bytes = offset & 0xf;

              const cleared = p & ~((1 << 0) | (1 << 1) | (1 << 2) | (1 << 3));

              // We must clear before setting the new value
              return cleared | bytes;
            });
          }}
        />
      </div>

      <div className="from-control">
        <label htmlFor="len">Lenght:</label>
        <input
          id="len"
          type="text"
          value={(combination >> 4) & 0xf}
          onChange={(e) => {
            const len = Number(e.target.value);

            setCombination((p) => {
              const cleared = p & ~((1 << 4) | (1 << 5) | (1 << 6) | (1 << 7));

              const bytes = (len & 0xf) << 4;

              return cleared | bytes;
            });
          }}
        />
      </div>

      <div className="from-control" style={{ columnGap: "10px" }}>
        <input
          id="reesult"
          disabled
          value={`0x${combination.toString(16)}`}
          style={{ textAlign: "right" }}
        />
        <button
          onClick={() =>
            navigator.clipboard.writeText(`0x${combination.toString(16)}`)
          }
        >
          copy
        </button>
      </div>
    </div>
  );
};
