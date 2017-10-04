using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Media.Imaging;

namespace StoreFrontPro.Server
{
   class Image
   {
      public int Index { get; private set; } = 0;

      private object mutex = new object();
      private bool loading = false;
      public bool Loading
      {
         get { lock(mutex) { return loading; } }
         set { lock(mutex) { loading = value; } }
      }

      public BitmapImage Item = null;
      public int Rank = 0;
      public string ImageGroup = null;

      public bool ContinueLoad = true;

      public Image(int aiIndex)
      {
         Index = aiIndex;
      }

      public void Set(BitmapImage image, string imageGroup)
      {
         Item = image;
         ImageGroup = imageGroup;
      }
   }

   class ImageLoadingObject
   {
      public Action<BitmapImage> ImageReceiver;
      public Image ImageLoad;
      public CardModel Model;

      public ImageLoadingObject( Image aImageObject,
                                 Action<BitmapImage> aCallback,
                                 CardModel aModel)
      {
         ImageLoad = aImageObject;
         ImageReceiver = aCallback;
         Model = aModel;
      }

      public void Loaded(BitmapImage Image, string FileGroup)
      {
         ImageLoad.Set(Image, FileGroup);

         ImageReceiver?.Invoke(ImageLoad.Item);
      }
   }

   class ImageHandler
   {
      static int Poolsize = 10;
      static Image[] ImagePool = new Image[Poolsize];
      static ImageHandler instance = null;
     
      public ImageHandler()
      {
         // Initialize the pool.
         for( int i = 0; i < Poolsize; i++ )
         {
            ImagePool[i] = new Image(i);
         }
      }

      /// <summary>
      /// This needs to only be called on ONE thread. Otherwise,
      /// we could get some weird image behavior.
      /// </summary>
      /// <param name="ImageReceiver"></param>
      public void GetImage(Action<BitmapImage> ImageReceiver, CardModel Model)
      {
         // Check if we already have the image.
         int iHave = getAlreadyPooled(Model.GetImagePath());
         if( iHave >= 0 )
         {
            shiftOnImage(iHave);
            ImageReceiver?.Invoke(ImagePool[iHave].Item);
            return;
         }

         // Overwrite an image.
         int iLRU = getLRU();
         if( iLRU >= 0 )
         {
            shiftOnImage(iLRU);

            Image imageLRU = ImagePool[iLRU];
            imageLRU.ImageGroup = Model.GetImagePath();

            ImageLoadingObject imageLO = new ImageLoadingObject(imageLRU, ImageReceiver, Model);

            startLoadImageThread(imageLO);
         }
         else
         {
            // Image request ignored.
         }
      }

      private void startLoadImageThread(ImageLoadingObject aImage)
      {
         aImage.ImageLoad.Loading = true;

         Thread loadThread = new Thread(loadImageThread);
         loadThread.IsBackground = true;
         loadThread.Start(aImage);
      }

      private void loadImageThread(object aImage)
      {
         ImageLoadingObject loadingImage = (ImageLoadingObject)aImage;

         if( loadingImage.ImageLoad.ContinueLoad )
         {
            string imageFile = downloadImage( loadingImage.Model );

            BitmapImage image = null;
            if( loadImageFromFile(imageFile, out image ) )
            {
               // imagefile should be the same as the model.getImagePath.
               loadingImage.Loaded(image, imageFile);
            }
         }

         loadingImage.ImageLoad.Loading = false;
      }

      private int getAlreadyPooled(string aszGroup)
      {
         for( int i = 0; i < Poolsize; i++ )
         {
            if( aszGroup == ImagePool[i].ImageGroup )
            {
               return i;
            }
         }

         return -1;
      }

      /// <summary>
      /// Gets the least recently used image.
      /// If all the images are in use, -1 is returned.
      /// </summary>
      /// <returns></returns>
      private int getLRU()
      {
         bool bSetIndex = false;
         int indexLowestRank = -1;
         int iLowestRank = Poolsize;
         for( int i = 0; i < Poolsize; i++ )
         {
            if( ImagePool[i].Rank < iLowestRank ) 
            {
               if( !ImagePool[i].Loading )
               {
                  iLowestRank = ImagePool[i].Rank;
                  indexLowestRank = i;
               }

               // Record that we tried to set the index.
               // This means we tried an failed, indicating
               // we have no available image.+

               bSetIndex = true;
            }
         }

         if( !bSetIndex )
         {
            indexLowestRank = 0;
         }

         return indexLowestRank;
      }

      private void shiftOnImage(int aiImage)
      {
         for( int i = 0; i < Poolsize; i++ )
         {
            if( i == aiImage )
            {
               ImagePool[i].Rank = Poolsize - 1;
            }
            else
            {
               ImagePool[i].Rank--;
            }
         }
      }

      /// <summary>
      /// Returns the file path of the image downloaded.
      /// </summary>
      /// <param name="aoCardModel"></param>
      /// <returns></returns>
      private string downloadImage(CardModel aoCardModel)
      {
         //Download the image.
         string szMUID = aoCardModel.GetAttr("multiverseid");
         string szFilePath = aoCardModel.GetImagePath();

         // Check if we already have the image
         string szDirectoryName = Path.GetDirectoryName(szFilePath);
         if (!Directory.Exists(szDirectoryName))
         {
            Directory.CreateDirectory(szDirectoryName);
         }

         // Less than 100 bytes, ignore the file.
         if( ( !File.Exists(szFilePath) ) ||
             ( new System.IO.FileInfo(szFilePath).Length < 100) )
         {
            if (File.Exists(szFilePath))
            {
               File.Delete(szFilePath);
            }

            using (WebClient client = new WebClient())
            {
               string szURL;
               if (!string.IsNullOrEmpty(szMUID))
               {
                  szURL = @"http://gatherer.wizards.com/Handlers/Image.ashx?multiverseid=" +
                          szMUID + @"&type=card";
               }
               else
               {
                  szURL = @"http://gatherer.wizards.com/Handlers/Image.ashx?name=" +
                          aoCardModel.Prototype + "&type=card";
               }

               // Download synchronously.
               try
               {
                  client.DownloadFile(new Uri(szURL, UriKind.RelativeOrAbsolute), szFilePath);
               }
               catch
               {
                  // Connection issues, various things can fail here
                  if (File.Exists(szFilePath)) { File.Delete(szFilePath); }
               }

            }
         }

         return szFilePath;
      }

      private bool loadImageFromFile(string aszFileName, out BitmapImage image)
      {
         if (!File.Exists(aszFileName))
         {
            image = null;
            return false;
         }

         try
         {
            BitmapImage bitmap = new BitmapImage();
            using (FileStream stream = File.OpenRead(aszFileName))
            {
               bitmap.BeginInit();
               bitmap.StreamSource = stream;
               bitmap.CacheOption = BitmapCacheOption.OnLoad;
               bitmap.EndInit();
               bitmap.Freeze();
            }
            image = bitmap;
            return true;
         }
         catch { }

         image = null;
         return false;
      }

      public static ImageHandler Handler
      {
         get
         {
            if( instance == null )
            {
               instance = new ImageHandler();
            }
            return instance;
         }
      }
   }
}
