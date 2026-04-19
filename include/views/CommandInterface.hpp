class CommandInterface {
    public:
        virtual ~CommandInterface() = default;
        
        virtual bool RollOtomatis() = 0;
};