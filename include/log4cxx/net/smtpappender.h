/***************************************************************************
                          smtpappender.h  -  class SMTPAppender
                             -------------------
    begin                : jeu mai 8 2003
    copyright            : (C) 2003 by Michael CATANZARITI
    email                : mcatan@free.fr
 ***************************************************************************/

/***************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.      *
 *                                                                         *
 * This software is published under the terms of the Apache Software       *
 * License version 1.1, a copy of which has been included with this        *
 * distribution in the LICENSE.txt file.                                   *
 ***************************************************************************/

#ifndef _LOG4CXX_NET_SMTP_H
#define _LOG4CXX_NET_SMTP_H

#include <helpers/appenderskeleton.h>

namespace log4cxx
{
	namespace net
	{
		/**
		Send an e-mail when a specific logging event occurs, typically on
		errors or fatal errors.

		<p>The number of logging events delivered in this e-mail depend on
		the value of <b>BufferSize</b> option. The
		<code>SMTPAppender</code> keeps only the last
		<code>BufferSize</code> logging events in its cyclic buffer. This
		keeps memory requirements at a reasonable level while still
		delivering useful application context.
		*/
		class SMTPAppender : public helpers::AppenderSkeleton
		{
		private:
			tstring to;
			tstring from;
			tstring subject;
			tstring smtpHost;
			int bufferSize; // 512
			bool locationInfo;
			CyclicBufferPtr cb;
			//Message msg;

		protected:
			TriggeringEventEvaluatorPtr evaluator;

		public:
		/**
		The default constructor will instantiate the appender with a
		spi::TriggeringEventEvaluator that will trigger on events with
		level ERROR or higher.*/
		SMTPAppender() ;

		/**
		Use <code>evaluator</code> passed as parameter as the 
		spi::TriggeringEventEvaluator for this net::SMTPAppender.
		*/
		SMTPAppender(TriggeringEventEvaluatorPtr evaluator);

		/**
		Activate the specified options, such as the smtp host, the
		recipient, from, etc.
		*/
		void activateOptions();


  /**
     Perform SMTPAppender specific appending actions, mainly adding
     the event to a cyclic buffer and checking if the event triggers
     an e-mail to be sent. */
  public
  void append(LoggingEvent event) {

    if(!checkEntryConditions()) {
      return;
    }

    event.getThreadName();
    event.getNDC();
    if(locationInfo) {
      event.getLocationInformation();
    }
    cb.add(event);
    if(evaluator.isTriggeringEvent(event)) {
      sendBuffer();
    }
  }

 /**
     This method determines if there is a sense in attempting to append.

     <p>It checks whether there is a set output target and also if
     there is a set layout. If these checks fail, then the boolean
     value <code>false</code> is returned. */
  protected
  boolean checkEntryConditions() {
    if(this.msg == null) {
      errorHandler.error("Message object not configured.");
      return false;
    }

    if(this.evaluator == null) {
      errorHandler.error("No TriggeringEventEvaluator is set for appender ["+
			 name+"].");
      return false;
    }


    if(this.layout == null) {
      errorHandler.error("No layout set for appender named ["+name+"].");
      return false;
    }
    return true;
  }


  synchronized
  public
  void close() {
    this.closed = true;
  }

  InternetAddress getAddress(String addressStr) {
    try {
      return new InternetAddress(addressStr);
    } catch(AddressException e) {
      errorHandler.error("Could not parse address ["+addressStr+"].", e,
			 ErrorCode.ADDRESS_PARSE_FAILURE);
      return null;
    }
  }

  InternetAddress[] parseAddress(String addressStr) {
    try {
      return InternetAddress.parse(addressStr, true);
    } catch(AddressException e) {
      errorHandler.error("Could not parse address ["+addressStr+"].", e,
			 ErrorCode.ADDRESS_PARSE_FAILURE);
      return null;
    }
  }

  /**
     Returns value of the <b>To</b> option.
   */
  public
  String getTo() {
    return to;
  }


  /**
     The <code>SMTPAppender</code> requires a {@link
     Layout layout}.  */
  public
  boolean requiresLayout() {
    return true;
  }

  /**
     Send the contents of the cyclic buffer as an e-mail message.
   */
  protected
  void sendBuffer() {

    // Note: this code already owns the monitor for this
    // appender. This frees us from needing to synchronize on 'cb'.
    try {
      MimeBodyPart part = new MimeBodyPart();

      StringBuffer sbuf = new StringBuffer();
      String t = layout.getHeader();
      if(t != null)
	sbuf.append(t);
      int len =  cb.length();
      for(int i = 0; i < len; i++) {
	//sbuf.append(MimeUtility.encodeText(layout.format(cb.get())));
	LoggingEvent event = cb.get();
	sbuf.append(layout.format(event));
	if(layout.ignoresThrowable()) {
	  String[] s = event.getThrowableStrRep();
	  if (s != null) {
	    for(int j = 0; j < s.length; j++) {
	      sbuf.append(s[j]);
	    }
	  }
	}
      }
      t = layout.getFooter();
      if(t != null)
	sbuf.append(t);
      part.setContent(sbuf.toString(), layout.getContentType());

      Multipart mp = new MimeMultipart();
      mp.addBodyPart(part);
      msg.setContent(mp);

      msg.setSentDate(new Date());
      Transport.send(msg);
    } catch(Exception e) {
      LogLog.error("Error occured while sending e-mail notification.", e);
    }
  }



  /**
     Returns value of the <b>EvaluatorClass</b> option.
   */
  public
  String getEvaluatorClass() {
    return evaluator == null ? null : evaluator.getClass().getName();
  }

  /**
     Returns value of the <b>From</b> option.
   */
  public
  String getFrom() {
    return from;
  }

  /**
     Returns value of the <b>Subject</b> option.
   */
  public
  String getSubject() {
    return subject;
  }

  /**
     The <b>From</b> option takes a string value which should be a
     e-mail address of the sender.
   */
  public
  void setFrom(String from) {
    this.from = from;
  }

  /**
     The <b>Subject</b> option takes a string value which should be a
     the subject of the e-mail message.
   */
  public
  void setSubject(String subject) {
    this.subject = subject;
  }


  /**
     The <b>BufferSize</b> option takes a positive integer
     representing the maximum number of logging events to collect in a
     cyclic buffer. When the <code>BufferSize</code> is reached,
     oldest events are deleted as new events are added to the
     buffer. By default the size of the cyclic buffer is 512 events.
   */
  public
  void setBufferSize(int bufferSize) {
    this.bufferSize = bufferSize;
    cb.resize(bufferSize);
  }

  /**
     The <b>SMTPHost</b> option takes a string value which should be a
     the host name of the SMTP server that will send the e-mail message.
   */
  public
  void setSMTPHost(String smtpHost) {
    this.smtpHost = smtpHost;
  }

  /**
     Returns value of the <b>SMTPHost</b> option.
   */
  public
  String getSMTPHost() {
    return smtpHost;
  }

  /**
     The <b>To</b> option takes a string value which should be a
     comma separated list of e-mail address of the recipients.
   */
  public
  void setTo(String to) {
    this.to = to;
  }



  /**
     Returns value of the <b>BufferSize</b> option.
   */
  public
  int getBufferSize() {
    return bufferSize;
  }

  /**
     The <b>EvaluatorClass</b> option takes a string value
     representing the name of the class implementing the 
     spi::TriggeringEventEvaluator interface. A corresponding object will
     be instantiated and assigned as the triggering event evaluator
     for the SMTPAppender.
   */
  public
  void setEvaluatorClass(String value) {
      evaluator = (TriggeringEventEvaluator)
                OptionConverter.instantiateByClassName(value,
					   TriggeringEventEvaluator.class,
						       evaluator);
  }


  /**
     The <b>LocationInfo</b> option takes a boolean value. By
     default, it is set to false which means there will be no effort
     to extract the location information related to the event. As a
     result, the layout that formats the events as they are sent out
     in an e-mail is likely to place the wrong location information
     (if present in the format).

     <p>Location information extraction is comparatively very slow and
     should be avoided unless performance is not a concern.
   */
  public
  void setLocationInfo(boolean locationInfo) {
    this.locationInfo = locationInfo;
  }

  /**
     Returns value of the <b>LocationInfo</b> option.
   */
  public
  boolean getLocationInfo() {
    return locationInfo;
  }
		}; // class SMTPAppender

		class DefaultEvaluator implements TriggeringEventEvaluator
		{
		public:
			/**
			Is this <code>event</code> the e-mail triggering event?
			<p>This method returns <code>true</code>, if the event level
			has ERROR level or higher. Otherwise it returns
			<code>false</code>.
			*/
			virtual bool isTriggeringEvent(const spi::LoggingEvent& event)
			{
    			return event.getLevel().isGreaterOrEqual(Level::ERROR);
  			}
		}; // class SMTPAppender
	}; // namespace net
}; // namespace log4cxx

#endif // _LOG4CXX_NET_SMTP_H
