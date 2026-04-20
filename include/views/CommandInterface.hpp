class CommandInterface {
    public:
        virtual ~CommandInterface() = default;
        
        virtual bool RollSettingan() = 0;

        virtual void cmd(std::string) = 0;

        virtual int GetInt() = 0;
};