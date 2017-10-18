using StoreFrontPro.Server;
using StoreFrontPro.Support.MultiDisplay;
using StoreFrontPro.Views.Components.CardImageDisplayer;
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
      #endregion

      #region Bindings
      private MultiDisplay _OperationWindow = new MultiDisplay();
      public MultiDisplay OperationWindow
      {
         get { return _OperationWindow; }
         set { _OperationWindow = value; OnPropertyChanged(); }
      }
      #endregion

      public VMCardImageDock(MCardImageDock Model, string RoutingName) 
         : base(Model, RoutingName)
      {
         Model.Register(this);
         SetDisplay("Nantuko Husk");
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

      public override void ModelUpdated()
      {
         ViewClass ImageDisplayer = ViewFactory.CreateCardImageViewer(
            Model: Model.ModelDisplay, 
            RoutingName: ImageDisplayName);

         OperationWindow.SetNewDisplay(
            Name: ImageDisplayName,
            NewDisplay: ImageDisplayer.View );
      }
   }
}
