using StoreFrontPro.Server;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media.Imaging;

namespace StoreFrontPro.Views.Components.CardImageDisplayer
{
   class VMCardImageDisplayer : ViewModel<CardModel>
   {
      private BitmapImage _CardImage = null;
      public BitmapImage CardImage
      {
         get { return _CardImage; }
         set { _CardImage = value; OnPropertyChanged(); }
      }

      public VMCardImageDisplayer(CardModel Model, string RoutingName) : base(Model, RoutingName)
      {
         Model.GetImage(imageLoaded);
      }

      private void imageLoaded(BitmapImage aBIImage)
      {
         CardImage = aBIImage;
      }

      #region IViewModel
      public override void ModelUpdated()
      {

      }
      #endregion
   }
}
