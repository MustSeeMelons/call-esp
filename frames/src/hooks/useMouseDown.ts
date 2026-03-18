import { useEffect, useState } from "react";

export const useMouseDown = () => {
  const [isMouseDown, setMouseDown] = useState(false);

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

  return isMouseDown;
};
