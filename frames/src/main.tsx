import { StrictMode } from "react";
import { createRoot } from "react-dom/client";
import App from "./App.tsx";
import { ModalContextProvider } from "./context/modal-context-provider.tsx";

createRoot(document.getElementById("root")!).render(
  <StrictMode>
    <ModalContextProvider>
      <App />
    </ModalContextProvider>
  </StrictMode>,
);
