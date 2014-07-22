
/*** block  from ../../../docs/manual/highlevel-xml.xml ***/
#include <stdlib.h>
#include <gst/gst.h>

gboolean playing;

int 
main (int argc, char *argv[]) 
{
  GstElement *filesrc, *osssink, *decode;
  GstElement *pipeline;

  gst_init (&argc,&argv);

  if (argc != 2) {
    g_print ("usage: %s <mp3 filename>\n", argv[0]);
    exit (-1);
  }

  /* create a new pipeline to hold the elements */
  pipeline = gst_element_factory_make ("pipeline", "pipeline");
  g_assert (pipeline != NULL);

  /* create a disk reader */
  filesrc = gst_element_factory_make ("filesrc", "disk_source");
  g_assert (filesrc != NULL);
  g_object_set (G_OBJECT (filesrc), "location", argv[1], NULL);

  /* and an audio sink */
  osssink = gst_element_factory_make ("osssink", "play_audio");
  g_assert (osssink != NULL);

  decode = gst_element_factory_make ("mad", "decode");
  g_assert (decode != NULL);

  /* add objects to the main pipeline */
  gst_bin_add_many (GST_BIN (pipeline), filesrc, decode, osssink, NULL);

  gst_element_link_many (filesrc, decode, osssink, NULL);

  /* write the pipeline to stdout */
  gst_xml_write_file (GST_ELEMENT (pipeline), stdout);

  /* write the bin to a file */
  gst_xml_write_file (GST_ELEMENT (pipeline), fopen ("xmlTest.gst", "w"));

  exit (0);
}
