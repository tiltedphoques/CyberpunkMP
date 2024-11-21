using CyberpunkSdk.Internal;

namespace CyberpunkSdk.Systems
{
    public class Logger
    {
        private ILogger logger;

        public Logger(string name)
        {
            logger = ILogger.Get(name);
        }

        public void Debug(string aMessage)
        {
            logger.Debug(aMessage);
        }

        public void Info(string aMessage)
        {
           logger.Info(aMessage);
        }

        public void Warn(string aMessage)
        {
            logger.Warn(aMessage);
        }

        public void Error(string aMessage)
        {
            logger.Error(aMessage);
        }
    }
}
