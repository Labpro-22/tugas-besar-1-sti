class CommandInterface {
    public:
        virtual ~CommandInterface() = default;
        
        virtual bool RollSettingan() = 0;

        virtual void cmd(std::string) = 0;

        virtual int GetInt() = 0;

        // Game setup
        // Game setup
        virtual std::string askFolderForConfig() = 0;
        virtual int askNumOfPlayer() = 0;
        virtual std::string askPlayerUsername() = 0;
        virtual bool askWantToLoadState() = 0;
};