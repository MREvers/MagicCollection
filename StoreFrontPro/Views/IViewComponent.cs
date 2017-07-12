using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Views
{
    #region Data Types
    public class DisplayEventArgs
    {
        public string Key;
        public object[] Args;

        public DisplayEventArgs(string Key, params object[] args)
        {
            this.Key = Key;
            this.Args = args;
        }
    }

    public class StoreFrontMenuItem
    {
        public RelayCommand Operation;
        public string MenuName;

        public StoreFrontMenuItem(string Name, Action<object> Operation)
        {
            this.Operation = new RelayCommand(Operation);
            this.MenuName = Name;
        }
    }
    #endregion

    #region Events & Delegates
    public delegate void DisplayEventHandler(object Source, DisplayEventArgs Event);
    #endregion

    interface IViewComponent
    {
        #region Interface
        event DisplayEventHandler DisplayEvent;

        List<StoreFrontMenuItem> GetMenuItems();
        #endregion
    }
}
