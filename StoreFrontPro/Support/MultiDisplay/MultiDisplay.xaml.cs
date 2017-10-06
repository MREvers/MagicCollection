using StoreFrontPro.Views;
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

namespace StoreFrontPro.Support.MultiDisplay
{
   /// <summary>
   /// Interaction logic for MultiDisplay.xaml
   /// </summary>
   public partial class MultiDisplay : UserControl, INotifyPropertyChanged, IViewComponent
   {
      #region Bindings
      private UserControl _Display = null;
      public UserControl Display
      {
         get { return _Display; }
         private set { _Display = value; OnPropertyChanged(); }
      }

      // Perhaps there is a way to assign a type here. Caller has to cast for now.
      private object _DisplayViewModel = null; // Assumes VM never chagnes per view.
      public object DisplayViewModel
      {
         get { return _DisplayViewModel; }
         private set { _DisplayViewModel = value; }
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

      private Action m_fnNewDisplayCallback = null;

      private Dictionary<string, UserControl> m_mapPersistantDisplays = new Dictionary<string, UserControl>();
      #endregion

      #region Events
      public event DisplayEventHandler DisplayEvent;
      #endregion

      #region Public Methods
      public MultiDisplay(Action NewDisplayCallback = null)
      {
         InitializeComponent();
         m_fnNewDisplayCallback = NewDisplayCallback;
         DataContext = this;
      }

      public void ShowOverlay(UserControl Overlay)
      {
         CloseOverlay();
         if (Overlay?.DataContext is IViewComponent)
         {
            (Overlay.DataContext as IViewComponent).DisplayEvent += displayFireEvent;
         }
         m_oOverlay = Overlay;
         displaySendToBack();
         fireUpdateMenuItems();
      }

      public void CloseOverlay()
      {
         displayBringToFront();
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
         if (Display?.DataContext is IViewComponent)
         {
            IViewComponent ivDisplay = Display.DataContext as IViewComponent;
            ivDisplay.DisplayEvent -= displayFireEvent;
         }

         if (Display?.DataContext is IViewModel)
         {
            (Display.DataContext as IViewModel).FreeModel();
         }

         DisplayName = Name;
         Display = NewDisplay;
         if (DataContext != null)
         {
            Display.DataContext = DataContext;
         }
         DisplayViewModel = Display.DataContext;

         if (DataContext is IViewComponent)
         {
            IViewComponent ivDisplay = DataContext as IViewComponent;
            ivDisplay.DisplayEvent += displayFireEvent;
         }

         if (m_fnNewDisplayCallback != null)
         {
            m_fnNewDisplayCallback();
         }

         fireUpdateMenuItems();
      }

      public void RecoverDisplay(string Name, bool Persist = false)
      {
         if (!m_mapPersistantDisplays.ContainsKey(Name)) { return; }

         SetNewDisplay(Name, m_mapPersistantDisplays[Name], Persist);

         m_mapPersistantDisplays.Remove(Name);
      }

      public List<StoreFrontMenuItem> GetMenuItems()
      {
         IEnumerable<StoreFrontMenuItem> lstRetVal = new List<StoreFrontMenuItem>();
         if (Display?.DataContext is IViewComponent)
         {
            lstRetVal = (Display.DataContext as IViewComponent).GetMenuItems();
         }

         if (m_oOverlay?.DataContext is IViewComponent)
         {
            lstRetVal = lstRetVal.Concat((m_oOverlay.DataContext as IViewComponent).GetMenuItems());
         }

         return lstRetVal.ToList();
      }
      #endregion

      #region Private Methods
      private void displaySendToBack()
      {
         if (m_oOverlay != null)
         {
            Display.IsEnabled = false;

            if( m_oOverlay.DataContext == null )
            {
               m_oOverlay.DataContext = new object();
            }

            // Note that this sets the data context of the overlay
            // to this multidisplay if it is null.
            MultiDisplayPanel.Children.Add(m_oOverlay);
            Panel.SetZIndex(m_oOverlay, 2);
         }
      }

      private void displayBringToFront()
      {
         Display.IsEnabled = true;
         MultiDisplayPanel.Children.Remove(m_oOverlay);
         if (m_oOverlay?.DataContext is IViewComponent)
         {
            (m_oOverlay.DataContext as IViewComponent).DisplayEvent -= displayFireEvent;
         }

         if (m_oOverlay?.DataContext is IViewModel)
         {
            (m_oOverlay.DataContext as IViewModel).FreeModel();
         }

         m_oOverlay = null;

         fireUpdateMenuItems();
      }

      private void fireUpdateMenuItems()
      {
         DisplayEventArgs eventArgs = new DisplayEventArgs(VCIMultiDisplay.Change);
         displayFireEvent(this, eventArgs);
      }

      private void displayFireEvent(object aoSource, DisplayEventArgs aoEvent)
      {
         if (DisplayEvent != null)
         {
            DisplayEvent(Event: aoEvent, Source: aoSource);
         }
      }
      #endregion
   }
}
