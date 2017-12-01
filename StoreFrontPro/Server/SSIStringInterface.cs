using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StoreFrontPro.Server
{
   partial class ServerInterface
   {
      public class StringIFace 
      {
         public List<string> ParseListDelimString(string aszDelim)
         {
            List<string> lstTemp = new List<string>();
            StringInterface.ParseListDelimString(aszDelim, ref lstTemp);

            return lstTemp;
         }


         public bool ParseCardString( string aszCardString, ref int riCount,
                                      ref string rszName,
                                      ref List<Tuple<string,string>> rlstDets, 
                                      ref List<Tuple<string,string>> rlstMeta ) 
         {
            return StringInterface.ParseCardString(aszCardString, ref riCount, ref rszName, ref rlstDets, ref rlstMeta);
         }
      }
   }
}
