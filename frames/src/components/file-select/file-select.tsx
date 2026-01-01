import "./file-select.scss";

export interface IFileSelectProps {
  labelNode: React.ReactNode;
  onChange: (file: File) => void;
}

export const FileSelect: React.FC<IFileSelectProps> = ({
  labelNode,
  onChange,
}) => {
  return (
    <div className="file-select">
      <input
        type="file"
        id="file-select"
        accept=".json"
        multiple={false}
        onChange={async (e) => {
          const files = e.target.files;

          if (files) {
            const file = files.item(0);
            file && onChange(file);
          }
        }}
      />
      <label htmlFor="file-select">{labelNode}</label>
    </div>
  );
};
