import { createContext, useContext, useState } from "react";

export interface IModal {
  id: string;
  node: React.ReactNode;
}

export interface IModalContext {
  modals: IModal[];
  pushModal: (id: string, node: React.ReactNode) => void;
  popModal: () => void;
}

const ModalContext = createContext<IModalContext>({
  modals: [],
  pushModal: () => {},
  popModal: () => {},
});

export const useModal = () => {
  const context = useContext(ModalContext);

  return context;
};

export type ModalProviderProps = React.HTMLProps<HTMLElement>;

export const ModalContextProvider: React.FC<ModalProviderProps> = ({
  children,
}) => {
  const [modals, setModals] = useState<IModal[]>([]);

  const pushModal = (id: string, node: React.ReactNode) => {
    if (!modals.some((m) => m.id === id)) {
      setModals((modals) => [...modals, { id, node }]);
    }
  };

  const popModal = () => {
    setModals((modals) => modals.slice(0, modals.length - 1));
  };

  return (
    // Add more controls as we need them
    <ModalContext.Provider value={{ modals: modals, popModal, pushModal }}>
      {children}
    </ModalContext.Provider>
  );
};
