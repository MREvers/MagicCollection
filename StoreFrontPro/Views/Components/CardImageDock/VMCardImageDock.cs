using StoreFrontPro.Server;
using StoreFrontPro.Support.MultiDisplay;
using StoreFrontPro.Views.Components.CardImageDisplayer;
using StoreFrontPro.Views.Interfaces.CardInterface;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static StoreFrontPro.Server.ServerInterface;

namespace StoreFrontPro.Views.Components.VCardImageDock
{
   class VMCardImageDock : ViewModel<MCardImageDock>
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
   }
}
