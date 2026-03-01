import { useModal } from "../../context/modal-context-provider";
import "./modal-container.scss";

export const ModalContainer = () => {
  const { modals, popModal } = useModal();

  return (
    <div
      id="modals"
      className={`modal-container ${modals.length > 0 ? "modal-open" : "modal-closed"}`}
      onClick={() => {
        popModal();
      }}
    >
      {modals.map((m, i) => {
        return (
          <div
            className="modal-wrapper"
            key={m.id}
            style={{ zIndex: 1 + 10 * i }}
            onClick={(e: React.MouseEvent) => e.stopPropagation()}
          >
            {m.node}
          </div>
        );
      })}
    </div>
  );
};
