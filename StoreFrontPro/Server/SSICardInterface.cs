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
    public partial class ServerInterface
    {
        public class CardIFace
        {
            public string SZ_IMAGE_CACHE_PATH = "";

            private class ImageDownloadedEventArgs : EventArgs
            {
                public ImageDownloadedEventArgs(CardModel aDataModel, EventArgs ae)
                {
                    DataModel = aDataModel;
                    e = ae;
                }
                public CardModel DataModel;
                public EventArgs e;
            }

            // Compares long name
            public bool AreCardsSame(CardModel aoCardOne, CardModel aoCardTwo)
            {
                if (aoCardOne.CardNameLong == null || aoCardTwo.CardNameLong == null)
                {
                    return false;
                }
                return SCI.IsSameIdentity(aoCardOne.CardNameLong, aoCardTwo.CardNameLong);
            }

            public bool AreMetaTagsSame(List<Tuple<string, string>> alstTupOne, List<Tuple<string, string>> alstTupTwo)
            {
                return SCI.IsSameMetaTags(alstTupOne, alstTupTwo);
            }

            public void AddMetaTag(string aszCollectionName,
                string aszLongCardName,
                string aszKey,
                string aszVal,
                List<Tuple<string, string>> aLstMatchMeta)
            {
                SCI.AddMetaTag(aszCollectionName, aszLongCardName, aszKey, aszVal, aLstMatchMeta);
            }

            public void SubmitMetaTagChangesToServer(string aszCollectionName, string aszLongName,
                List<Tuple<string, string>> alstNewMeta, List<Tuple<string, string>> alstMeta)
            {
                SCI.SetMetaTags(aszCollectionName, aszLongName, alstNewMeta, alstMeta);
            }

            public void SubmitFeatureChangesToServer(string aszCollectionName, string aszLongName,
                List<Tuple<string, string>> alstNewMeta,
                List<Tuple<string, string>> alstNewAttrs,
                List<Tuple<string, string>> alstMeta)
            {
                SCI.SetFeatures(aszCollectionName, aszLongName, alstNewMeta, alstNewAttrs, alstMeta);
            }

            public void RemoveMetaTag(string aszCollectionName,
                string aszLongCardName,
                string aszKey,
                List<Tuple<string, string>> aLstMatchMeta)
            {
                SCI.RemoveMetaTag(aszCollectionName, aszLongCardName, aszKey, aLstMatchMeta);
            }

            // Passes the image to the callback.
            // There is notably a memory leak in wpf bitmap images. read here.
            //http://stackoverflow.com/questions/21877221/memory-not-getting-released-in-wpf-image
            // I used the ANTS profiler and it looks like I am not leaking memory. It just seems that
            // the process will hold onto memory after its no longer needed. So when viewing lots of images
            // we may not expect it to return to its previous usage.
            public void DownloadAndCacheImage(Action<BitmapImage> aCallback, CardModel aoCardModel)
            {
                Thread downloadAndLoadImageThread = new Thread(()=> { inDownloadAndCacheImage(aCallback, aoCardModel); });
                downloadAndLoadImageThread.IsBackground = true;
                downloadAndLoadImageThread.Start();
            }

            private void inDownloadAndCacheImage(Action<BitmapImage> aCallback, CardModel aoCardModel)
            {
                //Download the image.
                string szMUID = aoCardModel.GetAttr("multiverseid");
                string szSet = aoCardModel.GetAttr("set");
                if (SZ_IMAGE_CACHE_PATH == "")
                {
                    SZ_IMAGE_CACHE_PATH = SCI.GetImagesPath();
                }
                string szBasePath = SZ_IMAGE_CACHE_PATH + "/_" + szSet + "/";
                string szFilePath = szBasePath + aoCardModel.CardName + ".jpg";

                // Check if we already have the image
                string szDirectoryName = Path.GetDirectoryName(szFilePath);
                if (!Directory.Exists(szDirectoryName))
                {
                    Directory.CreateDirectory(szDirectoryName);
                }

                if (!File.Exists(szFilePath))
                {
                    using (WebClient client = new WebClient())
                    {
                        string szURL;
                        if (!string.IsNullOrEmpty(szMUID))
                        {
                            szURL = @"http://gatherer.wizards.com/Handlers/Image.ashx?multiverseid=" + szMUID + @"&type=card";
                        }
                        else
                        {
                            szURL = @"http://gatherer.wizards.com/Handlers/Image.ashx?name=" + aoCardModel.CardName + "&type=card";
                        }

                        // Download synchronously.
                        client.DownloadFile(new Uri(szURL, UriKind.RelativeOrAbsolute), szFilePath);
                    }
                }

                loadImageFromFile(szFilePath, aCallback);
            }

            // Use sparingly. Eventually include attributes in metatags?
            public List<string> GetCardAttributesRestrictions(string aszLongName, string aszKey)
            {
                return SCI.GetCardAttributeRestrictions(aszLongName, aszKey);
            }

            private void loadImageFromFile(string aszFileName, Action<BitmapImage> aCallback)
            {
                if (!File.Exists(aszFileName)) { return; }

                BitmapImage bitmap = new BitmapImage();

                using (FileStream stream = File.OpenRead(aszFileName))
                {
                    bitmap.BeginInit();
                    bitmap.StreamSource = stream;
                    bitmap.CacheOption = BitmapCacheOption.OnLoad;
                    bitmap.EndInit();
                    bitmap.Freeze();
                }

                aCallback(bitmap);
            }
        }
    }
}
