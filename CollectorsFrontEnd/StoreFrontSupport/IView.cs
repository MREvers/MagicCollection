using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;

namespace CollectorsFrontEnd.StoreFrontSupport
{
    #region Data Types

    public class DisplayEventArgs
    {
        public string Source;
        public string Property;
        public string Event;

        private Dictionary<string, object> m_lstParms = new Dictionary<string, object>();

        public DisplayEventArgs(string Source, string Property, string Event)
        {
            this.Source = Source;
            this.Property = Property;
            this.Event = Event;
        }

        public void Add(string Key, object Value)
        {
            m_lstParms.Add(Key, Value);
        }

        public object Get(string aszKey)
        {
            return m_lstParms.ContainsKey(aszKey) ? m_lstParms[aszKey] : null;
        }
    }

    #endregion

    #region Enumerations

    public enum ViewComponentHandles
    {
        MenuItem,
        Buttons
    }

    #endregion

    #region Events & Delegates

    public delegate void DisplayEventHandler(object Source, DisplayEventArgs Event);

    #endregion

    interface IView
    {
        #region Interface

        // Fires whenever the VC cannot handle a UI Event.
        /// <summary>
        /// Views should fire the event as "<ComponentName>.<VarName>.<Event>"
        /// </summary>
        event DisplayEventHandler DisplayEvent;

        // Contains things like menu-items and button events not handled by the VC.
        List<object> GetViewHook(ViewComponentHandles ViewHandle);

        #endregion
    }
}
