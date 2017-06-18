using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace CollectorsFrontEnd.StoreFrontSupport
{
    /// <summary>
    /// Interaction logic for MultiDisplay.xaml
    /// Views that utilize this class have a few requirements for full functionality.
    /// One requirement is the the utilizing class must implement a displayEvent handler.
    /// The display will fire events that it can't handle so that the 'real' view may
    /// handle it as it pleases.
    /// </summary>
    public partial class MultiDisplay : UserControl, INotifyPropertyChanged, IView
    {

        #region Bindings

        private UserControl _Display = null;
        public UserControl Display
        {
            get { return _Display; }
            set { _Display = value; OnPropertyChanged(); }
        }

        public event PropertyChangedEventHandler PropertyChanged;

        protected virtual void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }

        #endregion Bindings

        #region Fields

        public string DisplayName = "";

        private UserControl m_oOverlay = null;

        private Dictionary<string, UserControl> m_mapPersistantDisplays = new Dictionary<string, UserControl>();

        #endregion

        #region Events
        public event DisplayEventHandler DisplayEvent;
        #endregion

        #region Public Methods

        public MultiDisplay()
        {
            InitializeComponent();

            DataContext = this;
        }

        public void ShowOverlay(UserControl Overlay)
        {
            m_oOverlay = Overlay;
            displaySendToBack();
        }

        public void SetNewDisplay(string Name, UserControl NewDisplay, bool Persist = false)
        {
            SetNewDisplay(Name, NewDisplay, null, Persist);
        }

        public void SetNewDisplay(string Name, UserControl NewDisplay, object DataContext, bool Persist)
        {
            if (Persist && Display != null)
            {
                m_mapPersistantDisplays.Add(DisplayName, Display);
            }

            // Need to make sure there are no events hooked on the old display.
            if (Display is IView)
            {
                IView ivDisplay = Display as IView;
                ivDisplay.DisplayEvent -= displayFireEvent;
            }

            DisplayName = Name;
            Display = NewDisplay;
            if (DataContext != null)
            {
                Display.DataContext = DataContext;
            }

            if (Display is IView)
            {
                IView ivDisplay = Display as IView;
                ivDisplay.DisplayEvent += displayFireEvent;
            }
        }

        public void RecoverDisplay(string Name, bool Persist = false)
        {
            if (!m_mapPersistantDisplays.ContainsKey(Name)) { return; }

            SetNewDisplay(Name, m_mapPersistantDisplays[Name], Persist);

            m_mapPersistantDisplays.Remove(Name);
        }

        public List<object> GetViewHook(ViewComponentHandles ViewHandle)
        {
            IView DisplayComponent = Display as IView;
            return DisplayComponent.GetViewHook(ViewHandle);
        }

        #endregion

        #region Private Methods

        private void displaySendToBack()
        {
            if (m_oOverlay != null)
            {
                Display.IsEnabled = false;
                CenterPanel.Children.Add(m_oOverlay);
                Panel.SetZIndex(m_oOverlay, 2);
            }
        }

        private void displayBringToFront()
        {
            Display.IsEnabled = true;
            CenterPanel.Children.Remove(m_oOverlay);
            m_oOverlay = null;
        }

        private void displayFireEvent(string aszEvent, UserControl aoSource)
        {
            DisplayEvent(Event: aszEvent, Source: aoSource);
        }

        #endregion
    }
}
