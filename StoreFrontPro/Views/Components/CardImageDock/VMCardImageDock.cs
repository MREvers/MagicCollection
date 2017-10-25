using StoreFrontPro.Server;
using StoreFrontPro.Support.MultiDisplay;
using StoreFrontPro.Views.Components.CardImageDisplayer;
using StoreFrontPro.Views.Components.CardImageDock;
using StoreFrontPro.Views.Interfaces.CardInterface;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using static StoreFrontPro.Server.ServerInterface;

namespace StoreFrontPro.Views.Components.VCardImageDock
{
   class VMCardImageDock : ViewModel<MCardImageDock>, IVCISupporter, IViewComponent
   {
      #region Names
      public const string ImageDisplayName = "IMGD";
      public const string DockDisplayName = "DOCK";
      #endregion

      #region Bindings
      private bool _CardChangerEnabled = true;
      public bool CardChangerEnabled
      {
         get { return _CardChangerEnabled; }
         set { _CardChangerEnabled = value; OnPropertyChanged(); }
      }

      private VMCardChanger _DockWindow;
      public VCardChanger DockWindow { get; set; }

      private MultiDisplay _ImageWindow = new MultiDisplay();
      public MultiDisplay ImageWindow
      {
         get { return _ImageWindow; }
         set { _ImageWindow = value; OnPropertyChanged(); }
      }
      #endregion

      public VMCardImageDock(MCardImageDock Model, string RoutingName) 
         : base(Model, RoutingName)
      {
         Model.Register(this);

         _DockWindow = new VMCardChanger(Model.ModelDisplay, DockDisplayName);
         _DockWindow.DisplayEvent += DisplayEventHandler;
         DockWindow = new VCardChanger() { DataContext = _DockWindow };

         SetDisplay("");
      }

      /// <summary>
      /// Corresponds to an actual model. Allows modification of model.
      /// </summary>
      /// <param name="Model"></param>
      public void SetDisplay(CardModel Model)
      {
         this.Model.SetDisplayImage(Model);
      }

      /// <summary>
      /// Creates a model that does not correspond to anything on the server.
      /// </summary>
      /// <param name="aszItemName"></param>
      public void SetDisplay(string aszItemName)
      {
         CardModel oTempModel = CardModel.GetPrototype(aszItemName).GetDefault();
         SetDisplay(oTempModel);
      }

      /// <summary>
      /// Changes the image displayed to match that of the model.
      /// This sets the model's display image, which, in turn,
      /// will call this class's ModelUpdated function.
      /// </summary>
      public void PreviewCard()
      {
         SetDisplay(Model.ModelDisplay);
      }

      /// <summary>
      /// Notify the owner of this class that the server state has changed.
      /// </summary>
      public void SubmitCard()
      {
         fireServerChanged();
      }

      /// <summary>
      /// The MCardImageDock Model received events from the parent. This is where its passed int.
      /// </summary>
      public override void ModelUpdated()
      {
         ViewClass ImageDisplayer = ViewFactory.CreateCardImageViewer(
            Model: Model.ModelDisplay, 
            RoutingName: ImageDisplayName);

         ImageWindow.SetNewDisplay(
            Name: ImageDisplayName,
            NewDisplay: ImageDisplayer.View );

         _DockWindow.SetEditting(Model.ModelDisplay);
      }

      #region Private Methods
      private void fireServerChanged()
      {
         DisplayEventArgs eArgs = new DisplayEventArgs(VCICardImageDock.SubmitChange);
         DisplayEvent?.Invoke(this, eArgs);
      }
      #endregion

      #region IViewComponent
      public event DisplayEventHandler DisplayEvent;

      public List<StoreFrontMenuItem> GetMenuItems()
      {
         throw new NotImplementedException();
      }
      #endregion

      #region IVCISupporter
      public void DisplayEventHandler(object source, DisplayEventArgs e)
      {
         if (!Application.Current.Dispatcher.CheckAccess())
         {
            Action aAction = new Action(() => { inDisplayEventHandler(source, e); });
            Application.Current.Dispatcher.BeginInvoke(aAction);
            return;
         }

         inDisplayEventHandler(source, e);
      }

      public void inDisplayEventHandler(object source, DisplayEventArgs e)
      {
         GetRouter().Call(source.GetType(), this, e.Key, e.Args);
      }

      public InterfaceRouter GetRouter()
      {
         return IRouter;
      }

      static InterfaceRouter _IRouter = null;
      static InterfaceRouter IRouter
      {
         get
         {
            if (_IRouter == null) { BuildInterface(); }
            return _IRouter;
         }
      }

      static void BuildInterface()
      {
         _IRouter = new InterfaceRouter();

         VCICardChanger CCIS = new VCICardChanger(
            Preview: (x) => { return (x as VMCardImageDock).PreviewCard; },
            Submit: (x) => { return (x as VMCardImageDock).SubmitCard; });
         _IRouter.AddInterface(CCIS);

      }
      #endregion
   }
}
