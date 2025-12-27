import { useSessionStorage } from "hooks-ts";
import { createContext, useMemo } from "react";

interface IThemeContext {
  theme: Theme;
  toggleTheme: () => void;
}

export enum Theme {
  Light,
  Dark,
}

type ThemeContextProps = {
  children: React.ReactNode;
};

export const ThemeContext = createContext<IThemeContext>({
  toggleTheme: () => {},
  theme: Theme.Dark,
});

export const ThemeProvider = ({ children }: ThemeContextProps) => {
  const [theme, setTheme] = useSessionStorage("theme", Theme.Dark);

  const applyTheme = () => {
    document.documentElement.setAttribute(
      "data-theme",
      theme === Theme.Dark ? "dark" : "light"
    );
  };

  const toggleTheme = () => {
    if (theme === Theme.Light) {
      setTheme(Theme.Dark);
    } else {
      setTheme(Theme.Light);
    }

    applyTheme();
  };

  const themeContextData: IThemeContext = useMemo(() => {
    applyTheme();

    return {
      theme,
      toggleTheme,
    };
  }, [toggleTheme]);

  return (
    <ThemeContext.Provider value={themeContextData}>
      {children}
    </ThemeContext.Provider>
  );
};
