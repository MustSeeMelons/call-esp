import { useState } from "react";
import "./playback.scss";
import { PIXEL_COUNT } from "../../definitions";

interface IPlaybackProps {
  frames: number[][];
}

// TODO finish this
export const Playback: React.FC<IPlaybackProps> = ({ frames }) => {
  const [frameCount] = useState(frames.length);
  const [frameIdx, setFrameIdx] = useState(0);

  const renderPixels = () => {
    const frameData = frames[frameIdx];
    const results: React.ReactNode[] = [];

    for (let i = 0; i < PIXEL_COUNT; i++) {
      const v = (frameData[i] >> i) & 1;

      results.push(
        <div
          key={`${frameIdx}-${i}`}
          className={`pixel${v ? " pixel-active" : ""}`}
        />,
      );
    }

    return results;
  };

  return <div className="playback">{renderPixels()}</div>;
};
